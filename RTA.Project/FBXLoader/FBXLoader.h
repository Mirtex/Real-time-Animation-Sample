// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FBXLOADER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FBXLOADER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef FBXLOADER_EXPORTS
#define FBXLOADER_API __declspec(dllexport)
#else
#define FBXLOADER_API __declspec(dllimport)
#endif

//#define FBXSDK_SHARED
#include "fbxsdk.h"
#include "FBXdata.h"


// This class is exported from the FBXLoader.dll
class FBXLOADER_API CFBXLoader : public FBXdata
{
private:

	//Tranforms
	struct Bone
	{
		FbxNode* thisNode = nullptr;
		int myIndex = 0;
		int parentIndex = -1;
		int jointIndex = -1;
	};

	struct BoneCluster
	{
		FbxVector4 myVert;
		std::vector<int> vIndices;	//Must be in the same order as vertexArray
		std::vector<float> weights;	//Must be in the same order as vertexArray
	};

	struct TransBone
	{
		float gTransform[16];
		int parentIndex = -1;
	};

	struct myKeyFrame
	{
		float time = 0;
		std::vector<XMFLOAT4X4> joints;
	};

	struct myClip
	{
		float duration;
		std::vector<myKeyFrame> keyFrames;
	};

	FbxSkeleton* m_Skeleton = nullptr;
	std::vector<Bone> skeletonBones;

	//Initialization Objects
	FbxManager* m_fbxManager = nullptr;
	FbxIOSettings *m_ioSettings = nullptr;
	FbxImporter* m_importer = nullptr;
	
	//Uber Data Object(s)
	FbxScene* m_Scene = nullptr;
	FbxPose* m_bindPose = nullptr; 
	FbxNode* m_Node = nullptr;
	FbxMesh* m_Mesh = nullptr;
	FbxSkin* m_Skinner = nullptr;
	unsigned int numPoses = 0;	   //For this class, always one
	unsigned int numBindItems = 0;

	//Model Data Objects
	unsigned int totalPolygons = 0;
	unsigned int numVertices = 0;
	unsigned int numIndices = 0;
	int* indexArray = nullptr;
	std::vector<BoneCluster> weightedVerts;
	FbxVector4* vertexArray;
	std::vector<int*> orderedIndex;		//Containers for broken up clusters, in order.
	std::vector<float*> orderedWeights;	//Containers for broken up clusters, in order.
	std::vector<FbxVector4> vxArray;	//Containers for broken up clusters, in order.
	
	std::vector<FbxVector4> normalArray;
	std::vector<FbxVector2> uvArray;
	//vertex data
	bool hasAnimation = false;
	int hasNormal = false;
	int hasUV = false;
	FbxGeometryElementNormal*	m_eNormals;
	FbxGeometryElementUV*		m_eUVs;
	//Animation Data Objects
	myClip m_animClip;
	FbxAnimStack* m_animStack = nullptr;
	FbxTimeSpan m_animTimeSpan;
	FbxTime m_animDuration;
	FbxLongLong m_frameCount = 0;
	
	std::string textureName;

	//Private Methods
	void BreadthFirst(FbxNode* child);

public:
	CFBXLoader();
	CFBXLoader(const char* filename, bool isAnimated = true);
	~CFBXLoader();

	bool GetModelData();

	void Terminate();
	void GetGridVertices(std::vector<DXEnvironment::VertexPositionColor>& myVerts);
	bool LoadFBX(std::vector<DXEnvironment::VertexPositionColor>& myVerts, std::vector<int>& outIndices, unsigned int &iCount);
	bool LoadJoints(std::vector<XMFLOAT4X4>& transBones, std::vector<int>& lineIndices);
	bool LoadAnimations();
	bool GetAnimationData(std::vector<std::vector<XMFLOAT4X4> >& animClip, std::vector<float>& times, float& duration);
	void GetTextureName(std::string& name) { name = textureName; textureName.clear(); };
	// TODO: add your methods here.
};

extern "C" FBXLOADER_API FBXdata* ActivateLoader(const char* filename, bool isAnimated);

//extern FBXLOADER_API int nFBXLoader;

//FBXLOADER_API int fnFBXLoader(void);
