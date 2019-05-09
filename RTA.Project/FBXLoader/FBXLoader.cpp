// FBXLoader.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include <iostream>
#include <string>
#include <queue>
#include "FBXLoader.h"

#define MATRIXSIZE 16


// This is an example of an exported variable
//FBXLOADER_API int nFBXLoader=0;

// This is an example of an exported function.

// This is the constructor of a class that has been exported.
// see FBXLoader.h for the class definition
CFBXLoader::CFBXLoader()
{
	
}

CFBXLoader::CFBXLoader(const char* filename, bool isAnimated)
{
	hasAnimation = isAnimated;
	m_fbxManager = FbxManager::Create();
	m_ioSettings = FbxIOSettings::Create(m_fbxManager, IOSROOT);
	m_fbxManager->SetIOSettings(m_ioSettings);
	m_importer = FbxImporter::Create(m_fbxManager, "");

	if (!m_importer->Initialize(filename, -1, m_ioSettings))
	{
		bool failure = true;
		Terminate();
		std::cout << " Error initializing importer" << std::endl;
	}

	m_Scene = FbxScene::Create(m_fbxManager, "teddyScene");

	m_importer->Import(m_Scene);
	m_importer->Destroy();
	m_importer = nullptr;

	//Get the name of the texture
	
	std::string tempName = ((FbxFileTexture*)m_Scene->GetTexture(0))->GetRelativeFileName();
	size_t i = tempName.find_last_of('\\') + 1;
	for (; i < tempName.size(); ++i)
	{
		textureName.push_back(tempName[i]);
	}
	i = textureName.find_last_of('.')+1;
	textureName[i]	 = 'd';
	textureName[i+1] = 'd';
	textureName[i+2] = 's';

	numPoses = m_Scene->GetPoseCount();
	
	for (unsigned int i = 0; i < numPoses; ++i)
		if (m_Scene->GetPose(i)->IsBindPose())
			m_bindPose = m_Scene->GetPose(i);

	//Test Variables 
	unsigned int meshCount = 0;
	unsigned int nullCount = 0;

	//Get the node with the mesh
	if (numPoses)
	{
		numBindItems = m_bindPose->GetCount();
		for (unsigned int i = 0; i < numBindItems; ++i)		
		{	
			if(!m_Mesh)
				m_Mesh = m_bindPose->GetNode(i)->GetMesh();
			if (!m_Skeleton)
				m_Skeleton = m_bindPose->GetNode(i)->GetSkeleton();

			if (m_Mesh && (m_Skeleton || !isAnimated))
			{
				//if (!isAnimated)
				//	break;

				if (m_Skeleton->IsSkeletonRoot())
					break;
				else
					m_Skeleton = nullptr;
			}
			// --------   Testing Data Below this loop-point (not gonna get there tho) -------- //
			if (m_Mesh)
				meshCount++;
			else
				nullCount++; 
		}				
		
		//Get the objects for skinning.
		for (int i = 0; i < m_Mesh->GetDeformerCount(); ++i)
		{
			if (m_Mesh->GetDeformer(i)->GetDeformerType() == FbxDeformer::eSkin)
			{
				m_Skinner = (FbxSkin*)m_Mesh->GetDeformer(i);
				break;
			}
		}

	}
	else
	{
		numPoses = m_Scene->GetNodeCount();

		for (unsigned int i = 0; i < numPoses; ++i)
		{
			if (!m_Mesh)
				m_Mesh = m_Scene->GetNode(i)->GetMesh();

			if (m_Mesh)
				break;

		}
		//m_Scene->GetGeometry();
	}
	


	//m_bindPose->GetNode(i);
	//FbxPose::GetCount()
	//FbxPose::GetNode(int pIndex)
	//FbxNode::GetMesh()
}

CFBXLoader::~CFBXLoader()
{
	if (m_fbxManager)
		m_fbxManager->Destroy();

	for (unsigned int i = 0; i < orderedIndex.size(); i++)
	{
		delete[] orderedIndex[i];
		delete[] orderedWeights[i];
	}

	// if(indexArray)
	// 	delete indexArray;

}

void CFBXLoader::BreadthFirst(FbxNode* root)
{
	//int childCount;// = m_Skeleton->GetNode()->GetChildCount();

	std::queue<Bone> list;
	
	Bone currBone;
	Bone childBone;
	unsigned int parentIndex = -1;
	unsigned int index = 0;
	//Bone parent;
	currBone.thisNode = root;
	currBone.parentIndex = parentIndex;
	currBone.myIndex = index;
	list.push(currBone);

	while (!list.empty())
	{
		currBone = list.front();
		list.pop();

		skeletonBones.push_back(currBone);

		for (int i = 0; i < currBone.thisNode->GetChildCount(); ++i)
		{
			
			++index;
			childBone.thisNode = currBone.thisNode->GetChild(i);
			childBone.parentIndex = currBone.myIndex;
			childBone.myIndex = index;

			list.push(childBone);
		}

	}


}

bool CFBXLoader::GetModelData()
{
	if (!m_Mesh)
		return false;

	//Get Vertex data
	numVertices = m_Mesh->GetControlPointsCount();
	totalPolygons = m_Mesh->GetPolygonCount();
	numIndices = m_Mesh->GetPolygonVertexCount();
	indexArray = m_Mesh->GetPolygonVertices();
	vertexArray = m_Mesh->GetControlPoints();

	//loading the verts in their indexed order.
	BoneCluster tempCluster;
	for (unsigned int i = 0; i < numVertices; ++i)
	{
		tempCluster.myVert = vertexArray[i];
		weightedVerts.push_back(tempCluster);

	}
	// A test to confirm the data we're getting. 
	// Verifies that the highest possible vertex index is the total count of vertices - 1 (zero based)
//#if defined(_DEBUG)
//	int test = 0;
//	unsigned int j = 0;
//	for (unsigned int i = 0; i < totalPolygons; ++i)
//	{
//		for (unsigned int j = 0; j < 3; ++j)
//		{
//			if (m_Mesh->GetPolygonVertex(i, j) > test)
//				test = m_Mesh->GetPolygonVertex(i, j); 
//
//		}
//		j < 3 ? ++j : j = 0;
//	}
//#endif
	hasNormal = m_Mesh->GetElementNormalCount();
	hasUV = m_Mesh->GetElementUVCount();
	//Get Joint data
	if (!m_Skeleton)
		return true;	//There's no skeleton, but we loaded a mesh.

	
	BreadthFirst(m_Skeleton->GetNode());
	if(hasAnimation)
		LoadAnimations();


	return true;
}


void CFBXLoader::Terminate()
{
	std::cout << "Terminating DLL.....  " << std::endl << std::endl;
	delete this;
}

void CFBXLoader::GetGridVertices(std::vector<DXEnvironment::VertexPositionColor>& myVerts)
{
	//std::vector<DXEnvironment::VertexPositionColor>* myData = new std::vector<DXEnvironment::VertexPositionColor>;
	//std::cout << "Test succesful" << std::endl;
	//system("pause");
	//DXEnvironment::VertexPositionColor temp = DXEnvironment::cubeVertices[0];
	//myData->push_back(temp);

	for (unsigned int i = 0; i < 4; ++i)
		myVerts.push_back(DXEnvironment::gridVerts[i]);
}

// If we got the data, pass it into our data structure.
bool CFBXLoader::LoadFBX(std::vector<DXEnvironment::VertexPositionColor>& myVerts, std::vector<int>& outIndices, unsigned int &iCount)
{
	if (!vertexArray)
		return false;

	if (hasAnimation)
	{

		FbxNode* currNode;
		FbxCluster* cluster;
		Bone* boneMatch;
		//Skinning data
		unsigned int testCounter = 0; //to make sure there are enough weights.
		for (int i = 0; i < m_Skinner->GetClusterCount(); ++i)
		{
			cluster = m_Skinner->GetCluster(i);
			currNode = cluster->GetLink();

			for (unsigned int x = 0; x < skeletonBones.size(); ++x)
			{
				if (currNode == skeletonBones[x].thisNode)
				{
					boneMatch = &skeletonBones[x];
					boneMatch->jointIndex = x;
					break;
				}
			}

			int* iVx = cluster->GetControlPointIndices();
			double* vxWeights = cluster->GetControlPointWeights();

			int clusterCount = cluster->GetControlPointIndicesCount();
			for (int x = 0; x < clusterCount; x++)
			{
				weightedVerts[iVx[x]].vIndices.push_back(boneMatch->jointIndex);
				weightedVerts[iVx[x]].weights.push_back(static_cast<float>(vxWeights[x]));
				testCounter++;
			}
		}
	}
	
	FbxGeometryElementNormal* normalElement	= nullptr;
	FbxGeometryElementUV* uvElement			= nullptr;
	//outIndices = new unsigned int[numIndices];
	FbxVector4 tempVertex;
	FbxVector4 tempNormal;
	FbxVector2 tempUV;
	float* tempWeights;
	int* tempIndices;
	if (hasUV)
		uvElement = m_Mesh->GetElementUV(0);
	if (hasNormal)
		normalElement = m_Mesh->GetElementNormal(0);


	for (unsigned int i = 0; i < numIndices; ++i)
	{
		//break up and order the clusters.
		if (hasAnimation)
		{
			tempIndices = new int[4];
			tempWeights = new float[4];

			for (unsigned int x = 0; x < 4; ++x)
			{
				tempIndices[x] = 0;
				tempWeights[x] = 0;
			}
			tempVertex = weightedVerts[indexArray[i]].myVert;
			size_t numWeights = weightedVerts[indexArray[i]].vIndices.size();
			for (unsigned int x = 0; x < numWeights; ++x)
			{
				tempIndices[x] = weightedVerts[indexArray[i]].vIndices[x];
				tempWeights[x] = weightedVerts[indexArray[i]].weights[x];
			}
			orderedWeights.push_back(tempWeights);
			orderedIndex  .push_back(tempIndices);
			vxArray		  .push_back(tempVertex);
		}
		else
		{
			tempVertex = weightedVerts[indexArray[i]].myVert;
			vxArray.push_back(tempVertex);
		}
		

		if (hasUV)
		{
			unsigned int tempi = i;
			if (uvElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
			{
				tempi = uvElement->GetIndexArray().GetAt(i);
			}
			tempUV = uvElement->GetDirectArray().GetAt(tempi);
			uvArray.push_back(tempUV); //Directly load to referenced containers instead.
		}
		if (hasNormal)
		{
			unsigned int tempi = i;
			if (normalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
			{
				tempi = normalElement->GetIndexArray().GetAt(i);
			}
			tempNormal = normalElement->GetDirectArray().GetAt(tempi);
			normalArray.push_back(tempNormal);
		}

		outIndices.push_back(indexArray[i]);
	}
	//Previous load (indexed.) the variables are below, move them up if a mess occurs.
	//for (unsigned int i = 0; i < numVertices; ++i)
	//{
	//	tempHolder.pos.x	= static_cast<float>(vertexArray[i][0]);
	//	tempHolder.pos.y	= static_cast<float>(vertexArray[i][1]);
	//	tempHolder.pos.z	= static_cast<float>(vertexArray[i][2]);
	//	myVerts.push_back(tempHolder);
	//	//tempHolder.color.x	= static_cast<float>();
	//	//tempHolder.color.y	= static_cast<float>();
	//	//tempHolder.color.z	= static_cast<float>();
	//}

	DXEnvironment::VertexPositionColor tempHolder;
	tempHolder.color = DirectX::XMFLOAT3(0.6f, 0.6f, 0.0f);
	for (unsigned int i = 0; i < numIndices; ++i)
	{
		tempHolder.pos.x = static_cast<float>(vxArray[i][0]);
		tempHolder.pos.y = static_cast<float>(vxArray[i][1]);
		tempHolder.pos.z = static_cast<float>(vxArray[i][2]);

		tempHolder.color.x	= static_cast<float>(uvArray[i][0]);
		tempHolder.color.y	= 1.0f - static_cast<float>(uvArray[i][1]);
		
		tempHolder.norm.x	= static_cast<float>(normalArray[i][0]);
		tempHolder.norm.y	= static_cast<float>(normalArray[i][1]);
		tempHolder.norm.z	= static_cast<float>(normalArray[i][2]);

		if (hasAnimation)
		{
			tempHolder.weights.x = orderedWeights[i][0];
			tempHolder.weights.y = orderedWeights[i][1];
			tempHolder.weights.z = orderedWeights[i][2];
			tempHolder.weights.w = orderedWeights[i][3];
			
			tempHolder.wJoints.x = orderedIndex[i][0];
			tempHolder.wJoints.y = orderedIndex[i][1];
			tempHolder.wJoints.z = orderedIndex[i][2];
			tempHolder.wJoints.w = orderedIndex[i][3];

			tempHolder.debugBool = 2;
		}

		//tempHolder.color.z	= static_cast<float>(uvArray[i][2]);
		//tempHolder.norm
		myVerts.push_back(tempHolder);
	}
	if(numIndices)
		iCount = numIndices;

	
	return true;
}

bool CFBXLoader::LoadJoints(std::vector<XMFLOAT4X4>& transBones, std::vector<int>& lineIndices)
{
	FbxAMatrix tempMatrix;
	XMFLOAT4X4 portMatrix;

	//transBones[1].m[0];
	for (unsigned int i = 0; i < skeletonBones.size(); ++i)
	{
		tempMatrix = skeletonBones[i].thisNode->EvaluateGlobalTransform();

		for (unsigned int i = 0; i < 4; ++i)
		{
			for (unsigned int j = 0; j < 4; ++j)
			{
				portMatrix.m[i][j] = static_cast<float>(tempMatrix.Get(i, j));
			}
		}

		transBones.push_back(portMatrix);
	}

	//Start from the first child.
	for (unsigned int i = 1; i < skeletonBones.size(); i++)
	{
		lineIndices.push_back(skeletonBones[i].myIndex);
		lineIndices.push_back(skeletonBones[i].parentIndex);
	}
	//Add arguments
	//Load data into the arguments 
	return true;
}

bool CFBXLoader::LoadAnimations()
{
	m_animStack = m_Scene->GetCurrentAnimationStack();
	m_animTimeSpan = m_animStack->GetLocalTimeSpan();
	m_animDuration = m_animTimeSpan.GetDuration();
	m_frameCount = m_animDuration.GetFrameCount(FbxTime::eFrames24);

	for (unsigned int i = 1; i < (unsigned int)m_frameCount; ++i)
	{
		myKeyFrame tempKey;
		m_animDuration.SetFrame(i, FbxTime::eFrames24);
		tempKey.time = static_cast<float>(m_animDuration.GetMilliSeconds());

		FbxAMatrix tempMatrix;
		XMFLOAT4X4 portMatrix;

		//transBones[1].m[0];
		for (unsigned int i = 0; i < skeletonBones.size(); ++i)
		{
			tempMatrix = skeletonBones[i].thisNode->EvaluateGlobalTransform(m_animDuration);

			for (unsigned int i = 0; i < 4; ++i)
			{
				for (unsigned int j = 0; j < 4; ++j)
				{
					portMatrix.m[i][j] = static_cast<float>(tempMatrix.Get(i, j));
				}
			}

			tempKey.joints.push_back(portMatrix);
			
		}
		m_animClip.keyFrames.push_back(tempKey);
	}
	m_animClip.duration = m_animClip.keyFrames[m_frameCount - 2].time; // This is probably wrong... Not sure what to put in there tho.

	return true;
}
bool CFBXLoader::GetAnimationData(std::vector<std::vector<XMFLOAT4X4 > >& animClip, std::vector<float>& times, float& duration)  // Pass the data to whoever wants it, if available.
{
	std::vector<std::vector<XMFLOAT4X4 > > tempClip;
	std::vector<float> tempTimes;

	unsigned int totalFrames = (unsigned int)m_animClip.duration - 1;

	for (unsigned int i = 0; i < (m_frameCount-1); ++i)
	{
		animClip.push_back(m_animClip.keyFrames[i].joints);
		times.push_back(m_animClip.keyFrames[i].time);
	}
	duration = m_animClip.duration;

	return true;
}


// ---------------- Activator -----------------------------
FBXLOADER_API FBXdata* ActivateLoader(const char* filename, bool isAnimated)
{
	FBXdata* myLoader;

	if(filename == nullptr)
		myLoader = new CFBXLoader("Teddy_Idle.fbx");
	else
 		myLoader = new CFBXLoader(filename, isAnimated);

    return myLoader;
}
