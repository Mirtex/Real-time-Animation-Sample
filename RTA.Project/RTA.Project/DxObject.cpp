#include <fstream>
#include <d3d11_1.h>

#include "DxObject.h"
#include "DDSTextureLoader.h"

#define TIMER_MILLIS 10
#define DDS_NAME L"Teddy_D.dds"
#define MAX_MODIFIER 20000.0
#define MIN_MODIFIER 0

using namespace std;

namespace DXEnvironment
{
	DxObject::DxObject()
	{
	}
	
	DxObject::DxObject(DxObjType myType)
	{
		objectType = myType;
	}

	/*******************************************************************************
	Notes: 
	1. Pass handles to Dx Instance to auto-load on device.
		R. Setting and Drawing currently through object, later can be through Instance.
	2. Logic to pass data to Dx Instance to know to call Draw or DrawIndexed.
	3. Complete Overload for incoming vertex data.
	
	After the buffer is set, find a way to set the device context 
	to draw through the interface itself.
	*******************************************************************************/

	DxObject::DxObject(DxInstance& myDX, bool indexed) // In Progress
	{
		LoadObject(myDX, indexed);
	}

	void DxObject::LoadObject(DxInstance& myDX, bool indexed, bool debug)
	{
		wireframeMode = debugMode = debug;
		
		if (!objectLoaded)
		{
			if (!instanceLoaded)
			{
				myInstance = &myDX;
				instanceLoaded = true;
			}
			
			isIndexed = indexed;
			//CreateBuffers(); // default is a Cube
			CreateShadersLayout(); //If I'm using one shader, this method might not need change.

			objectLoaded = true;

		}
		else
		{
			//clear data and Load a new object. Right now... I don't need this...
		}

		if (debugMode)
		{
			CreateJCBuffer();
		}
	}

	// Receive a standarized data set and load into the vertex buffer;
	//Not yet completed.
	DxObject::DxObject(DxInstance& myDX, std::vector<VertexPositionColor>& vertices, bool indexed, bool debug)
	{
		myInstance = &myDX;
		debugMode = debug;

		//Not Complete, should not load cube.
		unsigned int stride = sizeof(&vertices[0]);
		auto cubeBufferDesc = CD3D11_BUFFER_DESC(stride, D3D11_BIND_VERTEX_BUFFER);
		D3D11_SUBRESOURCE_DATA cubeData = { 0 };
		cubeData.pSysMem = &vertices[0];

		myDX.GetDevice()->CreateBuffer(&cubeBufferDesc, &cubeData, m_vertexBuffer.GetAddressOf());

		if (isIndexed)
		{
			//Create index buffer and store important data.
		}
	}

	DxObject::~DxObject()
	{
	}

	bool DxObject::LoadAnimation() // For Later: ReplaceAnimation(), as this method doesn't accept changes.
	{
		if (isAnimated)
			return false;

		isAnimated = true;
		//Load animation data
		
		return true;
	}

	bool DxObject::TranformObject(XMFLOAT4X4 transform, VertexPositionColor* vertex, unsigned int size, bool customObject)
	{
		if (!customObject) // No info
		{
			XMFLOAT4 temp;
			XMFLOAT4X4 tempM;
			float test = transform._42;
			for (unsigned int i = 0; i < ARRAYSIZE(transformedCube); ++i)
			{
				XMVECTOR vPos = XMLoadFloat4(&XMFLOAT4(transformedCube[i].pos.x, transformedCube[i].pos.y, transformedCube[i].pos.z, 1));
				XMStoreFloat4(&temp, XMVector4Transform(vPos, XMMatrixTranslation(transform._41, transform._42, transform._43)));
				XMStoreFloat4x4(&tempM, XMMatrixTranslation(transform._41, transform._42, transform._43));
				transformedCube[i].pos.x = temp.x;
				transformedCube[i].pos.y = temp.y;
				transformedCube[i].pos.z = temp.z;
				//transformedCube[i].debugBool = true;
			}
			return true;
		}
		else
		{
			//XMFLOAT4 temp;
			//XMFLOAT4X4 tempM;
			//float test = transform._42;
			//XMMATRIX transformMatrix = XMLoadFloat4x4(&transform);
			//for (unsigned int i = 0; i < size; ++i)
			//{
			//	XMVECTOR vPos = XMLoadFloat4(&XMFLOAT4(vertex[i].pos.x, vertex[i].pos.y, vertex[i].pos.z, 1));
			//	XMStoreFloat4(&temp, XMVector4Transform(vPos, transformMatrix));
			//	XMStoreFloat4x4(&tempM, XMMatrixTranslation(transform._41, transform._42, transform._43));
			//	vertex[i].pos.x = temp.x;
			//	vertex[i].pos.y = temp.y;
			//	vertex[i].pos.z = temp.z;
			//}
			//return true;

		}


		return true;
	}


	bool DxObject::CreateBuffers(bool customObject, VertexPositionColor* myPoints, unsigned int size, unsigned int* myIndices, unsigned int indexSize)
	{
		if (!customObject) // No info, draw something (Cube).
		{
			//Transform the Cube to appear on the root transform (test)

			unsigned int stride = sizeof(transformedCube);
			//CD3D11_BUFFER_DESC cubeBufferDesc(stride, D3D11_BIND_VERTEX_BUFFER);
			auto cubeBufferDesc = CD3D11_BUFFER_DESC(stride, D3D11_BIND_VERTEX_BUFFER);
			D3D11_SUBRESOURCE_DATA cubeData = { 0 };
			cubeData.pSysMem = transformedCube;

			HRESULT test = myInstance->GetDevice()->CreateBuffer(&cubeBufferDesc, &cubeData, m_vertexBuffer.GetAddressOf());

			if (isIndexed)
			{
				m_indexCount = ARRAYSIZE(cubeIndices);

				D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
				indexBufferData.pSysMem = cubeIndices;
				indexBufferData.SysMemPitch = 0;
				indexBufferData.SysMemSlicePitch = 0;
				unsigned int test = sizeof(cubeIndices);
				CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);

				myInstance->GetDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, m_indexBuffer.GetAddressOf());
			}
			
			m_vertexCount = ARRAYSIZE(transformedCube);


		}
		else
		{
			// Do we have data? if not, store it
			// We should use private data, this is a placeholder.
			if (!myVertices.size())
				for (unsigned int i = 0; i < size; ++i)
				{
					myVertices.push_back(myPoints[i].pos);
					myUVs.push_back(XMFLOAT2(myPoints[i].color.x, myPoints[i].color.y));
					myNormals.push_back(XMFLOAT4(myPoints[i].norm.x, myPoints[i].norm.y, myPoints[i].norm.z, 0));
				}

			if (objectType == Object)
				OffsetObject(myPoints);
			
			if(!this->myIndices.size())
				for (unsigned int i = 0; i < indexSize; i++)
					this->myIndices.push_back(myIndices[i]);
			
			unsigned int stride = sizeof(VertexPositionColor)*size;
			//CD3D11_BUFFER_DESC cubeBufferDesc(stride, D3D11_BIND_VERTEX_BUFFER);
			auto cubeBufferDesc = CD3D11_BUFFER_DESC(stride, D3D11_BIND_VERTEX_BUFFER);
			D3D11_SUBRESOURCE_DATA cubeData = { 0 };
			cubeData.pSysMem = myPoints;

			HRESULT test = myInstance->GetDevice()->CreateBuffer(&cubeBufferDesc, &cubeData, m_vertexBuffer.GetAddressOf());

			if (isIndexed)
			{
				m_indexCount = indexSize;

				D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
				indexBufferData.pSysMem = myIndices;
				indexBufferData.SysMemPitch = 0;
				indexBufferData.SysMemSlicePitch = 0;
				//unsigned int uintSize = sizeof(unsigned int);
				//unsigned int 
				CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned int)*indexSize, D3D11_BIND_INDEX_BUFFER);

				myInstance->GetDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, m_indexBuffer.GetAddressOf());
			}

			m_vertexCount = size;

			if (debugMode)
			{
				InitializeDebug();
			}
		}

		return true;
	}

	bool DxObject::CreateShadersLayout(bool customObject)
	{
		if (!customObject) // It's using Sample, so it's not a custom object, no extra shaders yet either.
		{
			//Daivuk's Tutorial method. Didn't Work for some reason.

			//ifstream vsFile("SampleVertexShader.cso", ios::binary);
			//ifstream psFile("SamplePixelShader.cso", ios::binary);
			//
			//vector<char> vsData = { istreambuf_iterator<char>(vsFile), istreambuf_iterator<char>()};
			//vector<char> psData = { istreambuf_iterator<char>(psFile), istreambuf_iterator<char>()};
			//
			//myInstance->GetDevice()->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &m_vertexShader);
			//myInstance->GetDevice()->CreatePixelShader(psData.data(), psData.size(), nullptr, &m_pixelShader);
			
			//END shader loading

			ID3DBlob *VS, *PS;
			HRESULT test;
			test = D3DCompileFromFile(L"SampleVertexShader.hlsl", 0, 0, "main", "vs_5_0", D3DCOMPILE_DEBUG, 0, &VS, 0);
			test = D3DCompileFromFile(L"SamplePixelShader.hlsl", 0, 0, "main", "ps_5_0", D3DCOMPILE_DEBUG, 0, &PS, 0);

			myInstance->GetDevice()->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &m_vertexShader);
			myInstance->GetDevice()->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &m_pixelShader);

			D3D11_INPUT_ELEMENT_DESC layout[] =
			{
				{ "POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "UV",				0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL",			0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "WEIGHTS",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "W_JOINTS",		0, DXGI_FORMAT_R32G32B32A32_SINT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "DEBUG_INDEX",	0, DXGI_FORMAT_R32_SINT,			0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "DEBUG_BOOL",		0, DXGI_FORMAT_R32_SINT,			0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			myInstance->GetDevice()->CreateInputLayout(layout, ARRAYSIZE(layout), VS->GetBufferPointer(), 
													   VS->GetBufferSize(), m_inputLayout.GetAddressOf());
			return true;
		}

		return false;
	}

	//bool DxObject::DirectXShaderPush()
	//{
	//
	//	return true;
	//}

	bool DxObject::FullLoad(std::vector<VertexPositionUVNormal> data)
	{
		return true;
	}

	bool DxObject::SetBuffers(bool fullVertex)	// Currently loading one vertex buffer only.
	{
		if (m_vertexBuffer == nullptr)
			return false;
		

		unsigned int stride = 0;
		unsigned int offset = 0;

		
		if (fullVertex) //not implemented yet
		{
			stride = sizeof(VertexPositionUVNormal);
			offset = 0;
		}
		else
		{
			stride = sizeof(VertexPositionColor);
			offset = 0;
		}
		//This vertex buffer might already be set when this is called, so...
		//ID3D11Buffer* temp; //... Get the very first buffer and check if it's the same.
		//myInstance->GetContext()->IAGetVertexBuffers(0, 1, &temp, &stride, &offset);
		//if (temp == m_vertexBuffer.Get())
		//	return true;


		myInstance->GetContext()->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		myInstance->GetContext()->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

		return true;
	}

	bool DxObject::SetShadersLayout()
	{
		auto deviceContext = myInstance->GetContext();

		//Restricted to initialization. 
		//Topology set to TriangleList; draws sets of triangles.
		if (m_vertexShader != nullptr && m_pixelShader != nullptr)
		{
			deviceContext->IASetInputLayout(m_inputLayout.Get());
			deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			deviceContext->VSSetShader(m_vertexShader.Get(), nullptr, NULL);
			deviceContext->PSSetShader(m_pixelShader.Get(), nullptr, NULL);
			return true;
		}
		return false;
	}

	
	bool DxObject::DrawObject()
	{

		SetBuffers();
		SetShadersLayout();

		if (m_txResView)
			SetTexture();

		myInstance->SetDebug(wireframeMode);

		if (m_vertexBuffer.Get() == nullptr || m_vertexShader.Get() == nullptr)
			return false;

		if (!debugMode || drawWireFrame)
		{
			if (isIndexed)
			{
				myInstance->GetContext()->DrawIndexed(m_indexCount, 0, 0);
			}
			else
			{
				myInstance->GetContext()->Draw(m_vertexCount, 0);
			}


		}
		
		if (debugMode && wireframeMode)
		{
			DrawTransformLines();
			DrawDebugLines();
		}

		return true;
	}

	bool DxObject::DrawTransformLines()
	{
		unsigned int stride = 0;
		unsigned int offset = 0;

		stride = sizeof(VertexPositionColor);
		offset = 0;

		myInstance->GetContext()->IASetVertexBuffers(0, 1, m_transformBuffer.GetAddressOf(), &stride, &offset);
		auto deviceContext = myInstance->GetContext();

		//Restricted to initialization. 
		//Topology set to TriangleList; draws sets of triangles.
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		myInstance->GetContext()->Draw(m_transformPtCount, 0);
		return true;
	}

	bool DxObject::DrawDebugLines()
	{
		unsigned int stride = sizeof(VertexPositionColor);
		unsigned int offset = 0;

		myInstance->GetContext()->IASetVertexBuffers(0, 1, m_boneBuffer.GetAddressOf(), &stride, &offset);
		myInstance->GetContext()->IASetIndexBuffer(m_lineIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		//Topology should have been set on previous method call.

		auto deviceContext = myInstance->GetContext();
		myInstance->GetContext()->DrawIndexed(m_lineIndexCount, 0, 0);

		return true;
	}

	void DxObject::InitializeDebug()
	{
		//Set up transform points.

		unsigned int stride = sizeof(VertexPositionColor)*myJoints.size()* ARRAYSIZE(debugTransformLines);
		//CD3D11_BUFFER_DESC cubeBufferDesc(stride, D3D11_BIND_VERTEX_BUFFER);
		auto transBufferDesc = CD3D11_BUFFER_DESC(stride, D3D11_BIND_VERTEX_BUFFER);
		D3D11_SUBRESOURCE_DATA transformData = { 0 };

		VertexPositionColor tempVert;
		
		for (unsigned int i = 0; i < myJoints.size(); i++)
		{
			for (unsigned int j = 0; j < ARRAYSIZE(debugTransformLines); ++j)
			{
				tempVert = debugTransformLines[j];
				tempVert.debugBool = 1;
				tempVert.debugIndex = i;
				TranformObject(myJoints[i], &tempVert, 1, true); 
				debugTransformStack.push_back(tempVert);
			}
		}
		if (myJoints.size())
		{

			transformData.pSysMem = &debugTransformStack[0];

			HRESULT test = myInstance->GetDevice()->CreateBuffer(&transBufferDesc, &transformData, m_transformBuffer.GetAddressOf());

			m_transformPtCount = ARRAYSIZE(debugTransformLines)*myJoints.size();
			m_transformCount = myJoints.size();

			//==============================================================================

			//Now set up joint lines

			stride = sizeof(VertexPositionColor)*(myJoints.size());

			auto lineBufferDesc = CD3D11_BUFFER_DESC(stride, D3D11_BIND_VERTEX_BUFFER);

			for (unsigned int i = 0; i < myJoints.size(); i++)
			{
				//tempVert.pos = XMFLOAT3(myJoints[i]._41, myJoints[i]._42, myJoints[i]._43);
				tempVert.pos = XMFLOAT3(0, 0, 0);

				tempVert.color = XMFLOAT3(0.0f, 1.0f, 1.0f);
				tempVert.debugBool = true;
				tempVert.debugIndex = i;
				debugLineStack.push_back(tempVert);

			}

			D3D11_SUBRESOURCE_DATA lineData = { 0 };
			lineData.pSysMem = &debugLineStack[0];

			test = myInstance->GetDevice()->CreateBuffer(&lineBufferDesc, &lineData, m_boneBuffer.GetAddressOf());

			//Get bone indices.
			stride = sizeof(unsigned int)*lineIndices.size();

			auto indexBufferDesc = CD3D11_BUFFER_DESC(stride, D3D11_BIND_INDEX_BUFFER);

			D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
			indexBufferData.pSysMem = &lineIndices[0];
			//unsigned int uintSize = sizeof(unsigned int);
			//unsigned int 
			//CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned int)*lineIndices.size(), D3D11_BIND_INDEX_BUFFER);

			m_lineIndexCount = lineIndices.size();

			myInstance->GetDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, m_lineIndexBuffer.GetAddressOf());
		}
	}

	void DxObject::ProcessInput(unsigned int key)
	{
		short nextFrame = 1;

		if (key == VK_RIGHT)	// Next key frame.
		{
			//nextFrame = 1;
			SwitchJoints(nextFrame);
			UpdateJCBuffer();
		}
		else if (key == VK_LEFT)  // Previous key frame.
		{
			nextFrame = -1;
			SwitchJoints(nextFrame);
			UpdateJCBuffer();

		}
		else if (key == 'Q')    // Disable skin polygons (wireframe or not.)
			DisableWireframe();
		else if (key == 'T')	// Enable Timed animation.
			StartTimer();
		else if (key == 'C')
			StopTimer();		// Disable Timed animation.
		else if (key == VK_TAB)
			wireframeMode = !wireframeMode; // Toggle wireframe mode.
		else if (key == VK_UP)
		{
			timerModifier += TIMER_MODIFIER_VALUE;
			if (timerModifier > MAX_MODIFIER) timerModifier = MAX_MODIFIER;
		}
		else if (key == VK_DOWN)
		{
			timerModifier -= TIMER_MODIFIER_VALUE;
			if (timerModifier < MIN_MODIFIER) timerModifier = MIN_MODIFIER;
		}
		else if (key == VK_F1)
		{
			std::string binName = "TeddyIDLE.bin";
			char* testHeader = new char[binName.size()];
			LoadBin(binName.c_str(), testHeader, binName.size());
			delete[] testHeader;
		}
		else if (key == VK_F2)
		{
			std::string binName = "TeddyBIN.bin";
			char* testHeader = new char[binName.size()];
			LoadBin(binName.c_str(), testHeader, binName.size());
			delete[] testHeader;
		}
		//else if (key == VK_F3)
		//{
		//	std::string binName = "TeddyATTACK1.bin";
		//	char* testHeader = new char[binName.size()];
		//	LoadBin(binName.c_str(), testHeader, binName.size());
		//	delete[] testHeader;
		//}
		//else if (key == VK_F4)
		//{
		//	std::string binName = "TeddyATTACK2.bin";
		//	char* testHeader = new char[binName.size()];
		//	LoadBin(binName.c_str(), testHeader, binName.size());
		//	delete[] testHeader;
		//}
	}

	void DxObject::UpdateVertexBuffers()
	{
		
	}

	void DxObject::DisableWireframe()
	{
		if (debugMode)
		{
			drawWireFrame = !drawWireFrame;
		}
	}

	bool DxObject::AssignJoint(int joint)
	{
		if (objectType == Object)
		{
			attachJoint = joint;
			isAttachment = true;

			return true;
		}
		//update transform model
		return false;
	}

	void DxObject::SwitchJoints(short direction)
	{
		cout << debugCount++ << endl;
		int tween1, tween2;
		//Testing Code
		if (!tweenTime)
		{
			m_currentFrame += direction;
		}
		else
		{
			if (m_currentFrame != (m_frameCount - 1))
			{
				tween1 = m_currentFrame;
				tween2 = m_currentFrame + 1;
			}
			else
			{
				tween1 = m_currentFrame;
				tween2 = 0;
			}
		}
		
		//Which ones are the parents of the tween.

		if (m_currentFrame < 0)
		{
			m_currentFrame = m_frameCount - 1;
		}
		else if (m_currentFrame >= m_frameCount)
		{
			m_currentFrame = 0;
			debugCount = 0;
		}

		if (m_transformCount)
		{
			if (!tweenTime)
			{
				for (unsigned int i = 0; i < m_transformCount; ++i)
				{
					XMFLOAT4X4 currTrans = allKeyFrames.keyFrames[m_currentFrame].joints[i];
					XMFLOAT4X4 tempFloats;
					XMMATRIX tempMatrix = XMLoadFloat4x4(&currTrans);
					tempMatrix = XMMatrixTranspose(tempMatrix);
					XMStoreFloat4x4(&tempFloats, tempMatrix);
					m_FrameContainer.transforms[i] = tempFloats;


					//VertexPositionColor tempVert;
					//tempVert.pos = XMFLOAT3(currTrans._41, currTrans._42, currTrans._43);
					//tempVert.color = XMFLOAT3(0.0f, 1.0f, 1.0f);
					//debugLineStack.push_back(tempVert);
					//tempVert.debugBool = false;
					//tempVert.debugIndex = i;

				}
			}
			else
			{
				//Calculate index for tweeners
				oneKeyFrame firstFrame, secFrame;
				firstFrame = allKeyFrames.keyFrames[tween1];
				secFrame = allKeyFrames.keyFrames[tween2];

				for (unsigned int i = 0; i < m_transformCount; ++i)
				{
					m_FrameContainer.transforms[i] = CalculateTween(firstFrame.joints[i], secFrame.joints[i]);

				}
			}
		}
		tweenTime = !tweenTime;

		//multiply inverse of bind pose with the m_frameCOntainer and assign the result to m_framecontainer.
		//and voila, multiplication matrix.
		CalculateSkinTransforms();

	}

	void DxObject::SwitchTimedJoints()
	{
		unsigned int direction = 1;
		int tween1 = 0, tween2 = 0;

		float keytime = 0;
		for (unsigned int i = 0; i < allKeyFrames.keyFrames.size(); ++i)
		{
			keytime = allKeyFrames.keyFrames[i].time;

			if (timerVal < keytime)
			{
				if(i == 0)
				{
					tween1 = allKeyFrames.keyFrames.size() - 1;
					tween2 = i;
					break;
				}
				else
				{
					tween1 = i - 1;
					tween2 = i;
					break;
				}
			}
			
		}

		//Calculate value t of tween
		//float relativeTime = 
		int tweenTime1 = tween2 == 0 ? 0 : allKeyFrames.keyFrames[tween1].time;
		float tweenT = (timerVal - tweenTime1)
							/	(allKeyFrames.keyFrames[tween2].time - tweenTime1);

		m_currentFrame = tween1;
		if (tween1 == tween2)
		{
			cout << "Error: tweens are the same. Exiting DxObject::SwitchTimedJoints() ... " << endl << endl;
			system("pause");
			return;
		}
		
		if (m_currentFrame < 0)
		{
			m_currentFrame = m_frameCount - 1;
		}
		else if (m_currentFrame >= m_frameCount)
		{
			m_currentFrame = 0;
			debugCount = 0;
		}

		if (m_transformCount)
		{
				//Calculate index for tweeners
				oneKeyFrame firstFrame, secFrame;
				firstFrame = allKeyFrames.keyFrames[tween1];
				secFrame = allKeyFrames.keyFrames[tween2];

				for (unsigned int i = 0; i < m_transformCount; ++i)
				{
					m_FrameContainer.transforms[i] = CalculateTween(firstFrame.joints[i], secFrame.joints[i], tweenT);

				}
		}

		CalculateSkinTransforms();
	}

	void DxObject::InitBufferBones()
	{
		if (myJoints.size())
		{
			for (unsigned int i = 0; i < m_transformCount; ++i)
			{
				XMFLOAT4X4 tempFloats;
				XMMATRIX tempMatrix = XMLoadFloat4x4(&myJoints[i]);
				tempMatrix = XMMatrixTranspose(tempMatrix);
				XMStoreFloat4x4(&tempFloats, tempMatrix);
				m_FrameContainer.transforms[i] = tempFloats;

			}
		}
	}

	bool DxObject::CreateJCBuffer()
	{
		if (m_constantBuffer.Get() != nullptr)
			return true;

		if (!myJoints.size())
			return false;

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(TransformStack), D3D11_BIND_CONSTANT_BUFFER);
		if (myInstance->GetDevice()->CreateBuffer(&constantBufferDesc, nullptr, &m_constantBuffer) == S_OK)
		{
			InitBufferBones();
			UpdateJCBuffer();
			return true;
		}

		return false;
	}

	bool DxObject::UpdateJCBuffer()
	{
		if (m_constantBuffer.Get() == nullptr)
		{
			return false;
		}

		// Commented: Debug Data

		//for (unsigned int i = 0; i < m_transformCount; ++i)
		//{
		//	cout << m_FrameContainer.transforms[i]._11 << " " << m_FrameContainer.transforms[i]._12 << " " << m_FrameContainer.transforms[i]._13 << " " << m_FrameContainer.transforms[i]._14 << endl;
		//	cout << m_FrameContainer.transforms[i]._21 << " " << m_FrameContainer.transforms[i]._22 << " " << m_FrameContainer.transforms[i]._23 << " " << m_FrameContainer.transforms[i]._24 << endl;
		//	cout << m_FrameContainer.transforms[i]._31 << " " << m_FrameContainer.transforms[i]._32 << " " << m_FrameContainer.transforms[i]._33 << " " << m_FrameContainer.transforms[i]._34 << endl;
		//	cout << m_FrameContainer.transforms[i]._41 << " " << m_FrameContainer.transforms[i]._42 << " " << m_FrameContainer.transforms[i]._43 << " " << m_FrameContainer.transforms[i]._44 << endl;
		//}
		m_FrameContainer.numPerFrame = m_transformCount;
		myInstance->GetContext()->UpdateSubresource(m_constantBuffer.Get(), 0, NULL, &m_FrameContainer, 0, 0);
		myInstance->GetContext()->VSSetConstantBuffers(1, 1, m_constantBuffer.GetAddressOf());
		
		return true;
	}

	void DxObject::SetBones(std::vector<XMFLOAT4X4> newBones)
	{ 
		myJoints = newBones; 
		m_transformCount = myJoints.size(); 
	}

	bool DxObject::LoadTexture()  //Maybe pass directory later. ->  ->  ->  vvvvv
	{
		wstring temp;
		for (int i = 0; i < textureName.size(); ++i)
			temp.push_back(textureName[i]);

		HRESULT result = CreateDDSTextureFromFile(myInstance->GetDevice(), 
													temp.c_str(), 
													(ID3D11Resource**)m_mainTexture.GetAddressOf(), 
													m_txResView.GetAddressOf());
		
		CD3D11_SAMPLER_DESC cubeSamplerDesc = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
		cubeSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		cubeSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		cubeSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		result = myInstance->GetDevice()->CreateSamplerState(&cubeSamplerDesc, m_mainSampler.GetAddressOf());
		
		return true;
	}

	bool DxObject::SetTexture()
	{
		ID3D11ShaderResourceView* resViews[] = { m_txResView.Get() };
		myInstance->GetContext()->PSSetShaderResources(0, 1, resViews);
		myInstance->GetContext()->PSSetSamplers(0, 1, m_mainSampler.GetAddressOf());
		return true;
	}

	//Timer methods
	void DxObject::StartTimer()
	{
		if (!timerOn)
		{
			timer = std::clock();
			timerOn = true;
		}
	}

	void DxObject::TimerCheck()
	{
		if (timerOn)
		{
			timerVal = timerModifier*(std::clock() - timer)/ CLOCKS_PER_SEC;
			if (timerVal < allKeyFrames.duration)
			{
				SwitchTimedJoints();
				UpdateJCBuffer();
			}
			else
			{
				//std::string binName = "TeddyBIN.bin";
				//char* testHeader = new char[binName.size()];
				//LoadBin(binName.c_str(), testHeader, binName.size());
				//delete[] testHeader;
				// End Directx code
				timer = std::clock();
			}
		}
	}

	void DxObject::StopTimer()
	{
		if (timerOn)
		{
			timerOn = false;
		}
	}

	//Calculates one tween from the passed in transforms w/Quaternion Slerp.
	XMFLOAT4X4 DxObject::CalculateTween(XMFLOAT4X4 preJoints, XMFLOAT4X4 postJoints, float slice)
	{
		//Rotation
		//XMFLOAT3X3 preRotation;
		//XMFLOAT3X3 postRotation;
		//for (unsigned short i = 0; i < 3; ++i)
		//{
		//	for (unsigned short j = 0; j < 3; ++j)
		//	{
		//		preRotation.m[i][j] = preJoints.m[i][j];
		//		postRotation.m[i][j] = postJoints.m[i][j];
		//	}
		//}
		XMMATRIX preMatrix	= XMLoadFloat4x4(&preJoints);
		XMMATRIX postMatrix = XMLoadFloat4x4(&postJoints);
		
		XMVECTOR preQt = XMQuaternionRotationMatrix(preMatrix);
		XMVECTOR postQt = XMQuaternionRotationMatrix(postMatrix);
		
		XMVECTOR tweenQt = XMQuaternionSlerp(preQt, postQt, slice);

		XMMATRIX tweenMatrix = XMMatrixRotationQuaternion(tweenQt);
		tweenMatrix = XMMatrixTranspose(tweenMatrix);

		XMFLOAT4X4 tweenTransform;
		XMStoreFloat4x4(&tweenTransform, tweenMatrix);
		
		//Position
		XMFLOAT4 preFloat;
		XMFLOAT4 postFloat;

		preFloat = XMFLOAT4(preJoints.m[3]);
		postFloat = XMFLOAT4(postJoints.m[3]);

		XMVECTOR prePos  =  XMLoadFloat4(&preFloat);
		XMVECTOR postPos =  XMLoadFloat4(&postFloat);
		XMVECTOR tweenPos = XMQuaternionSlerp(prePos, postPos, slice);

		XMFLOAT4 tweenFloat;
		XMStoreFloat4(&tweenFloat, tweenPos);
		
		tweenTransform.m[0][3] = tweenFloat.x;
		tweenTransform.m[1][3] = tweenFloat.y;
		tweenTransform.m[2][3] = tweenFloat.z;
		tweenTransform.m[3][3] = tweenFloat.w;
		//preFloat  = XMFLOAT4(preJoints .m[3],preJoints .m[3],preJoints .m[3],preJoints .m[3]);
		//postFloat = XMFLOAT4(postJoints.m[3],postJoints.m[3],postJoints.m[3],postJoints.m[3]);
		
		return tweenTransform;
	}

	bool DxObject::CalculateSkinTransforms()
	{
		if (myBindPose.size() && myJoints.size() && (myBindPose.size() == myJoints.size()))
		{
			XMMATRIX aniMatrix;
			XMMATRIX bindMatrix;
			for (unsigned int i = 0; i < myBindPose.size(); i++)
			{
				aniMatrix = XMLoadFloat4x4(&m_FrameContainer.transforms[i]);
				bindMatrix = XMLoadFloat4x4(&myBindPose[i]);
				aniMatrix = XMMatrixMultiply(aniMatrix, XMMatrixInverse(nullptr, XMMatrixTranspose(bindMatrix)));

				XMStoreFloat4x4(&m_FrameContainer.vTransforms[i], aniMatrix);
			}

		}
		return true;
	}

	void DxObject::SetClip(animClip temp)
	{ 
		allKeyFrames = temp; 
		isAnimated = true; 
		m_frameCount = allKeyFrames.keyFrames.size();

	}

	bool DxObject::OffsetObject(VertexPositionColor* myPts)
	{

		XMVECTOR tempVector;
		XMMATRIX tempMatrix;
		tempMatrix = XMMatrixTranslation(0,0,-40.5f);
		tempMatrix = XMMatrixMultiply(tempMatrix, XMMatrixScaling(0.5f, 0.5f, 0.5f));
		tempMatrix = XMMatrixMultiply(tempMatrix, XMMatrixRotationX(-90.0f));


		
		for (unsigned int i = 0; i < myVertices.size(); ++i)
		{
			tempVector = XMLoadFloat3(&myVertices[i]);
			tempVector = XMVector3Transform(tempVector, tempMatrix);
			XMStoreFloat3(&myVertices[i], tempVector);
			XMStoreFloat3(&myPts[i].pos, tempVector);
			//store it in constant buffer container
		}
		return true;
	}

	void DxObject::SetTransformOffset(XMFLOAT4X4 newJoint)
	{
		attachTransform = newJoint;
	}

	bool DxObject::SaveBin(const char* filePath, const char* headerChunk, int numberOfBytes)
	{
		char* tempName = new char[numberOfBytes];
		for (unsigned int i = 0; i < numberOfBytes; i++)
			tempName[i] = headerChunk[i];

		ofstream outFile;

		outFile.open(filePath, ios_base::binary);

		//Test Start
		cout << " First Vertex before load: ";
		cout << myVertices[0].x << " " << myVertices[0].y << " " << myVertices[0].z << " " << endl;
		cout << " First Normal before load: ";
		cout << myNormals[0].x << " " << myNormals[0].y << " " << myNormals[0].z << " " << myNormals[0].w << " " << endl;
		cout << " First UV before load: ";
		cout << myUVs[0].x << " " << myUVs[0].y << endl;
		cout << " First Index before load: ";
		cout << myIndices[0] << endl << endl;//Test End
		//Test End

		if (outFile.is_open())
		{
			outFile.write(tempName, numberOfBytes);

			//Write notifiers
			outFile.write((char*)&objectLoaded, sizeof(objectLoaded));
			outFile.write((char*)&instanceLoaded, sizeof(instanceLoaded));
			outFile.write((char*)&debugMode, sizeof(debugMode));
			outFile.write((char*)&wireframeMode, sizeof(wireframeMode));
			outFile.write((char*)&drawWireFrame, sizeof(drawWireFrame));

			
			//Bind Pose; write/read size first
			unsigned int size = myVertices.size();
			float tempX, tempY, tempZ;
			outFile.write((char*)&size, sizeof(unsigned int));
			for (unsigned int i = 0; i < size; i++)
			{
				tempX = myVertices[i].x;
				tempY = myVertices[i].y;
				tempZ = myVertices[i].z;
				outFile.write((char*)&tempX, sizeof(float));
				outFile.write((char*)&tempY, sizeof(float));
				outFile.write((char*)&tempZ, sizeof(float));
			}

			size = myBindPose.size();
			float temp[16] = { 0 };

			outFile.write((char*)&size, sizeof(unsigned int));
			for (unsigned int i = 0; i < size; i++)
			{
				temp[0]  = myBindPose[i].m[0][0];
				temp[1]  = myBindPose[i].m[0][1];
				temp[2]  = myBindPose[i].m[0][2];
				temp[3]  = myBindPose[i].m[0][3];
				temp[4]  = myBindPose[i].m[1][0];
				temp[5]  = myBindPose[i].m[1][1];
				temp[6]  = myBindPose[i].m[1][2];
				temp[7]  = myBindPose[i].m[1][3];
				temp[8]  = myBindPose[i].m[2][0];
				temp[9]  = myBindPose[i].m[2][1];
				temp[10] = myBindPose[i].m[2][2];
				temp[11] = myBindPose[i].m[2][3];
				temp[12] = myBindPose[i].m[3][0];
				temp[13] = myBindPose[i].m[3][1];
				temp[14] = myBindPose[i].m[3][2];
				temp[15] = myBindPose[i].m[3][3];

				for (unsigned int x = 0; x < 16; x++)
				{
				outFile.write((char*)&temp[x], sizeof(float));
				}

			}

			size = myJoints.size();
			outFile.write((char*)&size, sizeof(unsigned int));
			for (unsigned int i = 0; i < size; i++)
			{
				temp[0]  = myBindPose[i].m[0][0];
				temp[1]  = myBindPose[i].m[0][1];
				temp[2]  = myBindPose[i].m[0][2];
				temp[3]  = myBindPose[i].m[0][3];
				temp[4]  = myBindPose[i].m[1][0];
				temp[5]  = myBindPose[i].m[1][1];
				temp[6]  = myBindPose[i].m[1][2];
				temp[7]  = myBindPose[i].m[1][3];
				temp[8]  = myBindPose[i].m[2][0];
				temp[9]  = myBindPose[i].m[2][1];
				temp[10] = myBindPose[i].m[2][2];
				temp[11] = myBindPose[i].m[2][3];
				temp[12] = myBindPose[i].m[3][0];
				temp[13] = myBindPose[i].m[3][1];
				temp[14] = myBindPose[i].m[3][2];
				temp[15] = myBindPose[i].m[3][3];

				for (unsigned int x = 0; x < 16; x++)
				{
					outFile.write((char*)&temp[x], sizeof(float));
				}

			}

			size = myNormals.size();
			float tempW;
			outFile.write((char*)&size, sizeof(unsigned int));
			for (unsigned int i = 0; i < size; i++)
			{
				tempX = myNormals[i].x;
				tempY = myNormals[i].y;
				tempZ = myNormals[i].z;
				tempW = myNormals[i].w;
				outFile.write((char*)&tempX, sizeof(float));
				outFile.write((char*)&tempY, sizeof(float));
				outFile.write((char*)&tempZ, sizeof(float));
				outFile.write((char*)&tempW, sizeof(float));
			}

			size = myUVs.size();
			outFile.write((char*)&size, sizeof(unsigned int));
			for (unsigned int i = 0; i < size; i++)
			{
				tempX = myUVs[i].x;
				tempY = myUVs[i].y;
				outFile.write((char*)&tempX, sizeof(float));
				outFile.write((char*)&tempY, sizeof(float));
			}

			size = myIndices.size();
			outFile.write((char*)&size, sizeof(unsigned int));
			int tInt = 0;
			outFile.write((char*)&myIndices[0], sizeof(int)*size); // cleaner...

			//Animation clip; first lose # clips, then size of clips (assumes same size of clips
			size = allKeyFrames.keyFrames.size();
			outFile.write((char*)&size, sizeof(unsigned int));

			unsigned int subSize = allKeyFrames.keyFrames[0].joints.size();
			outFile.write((char*)&subSize, sizeof(unsigned int));

			for (unsigned int i = 0; i < size; i++)
			{
				for (unsigned int j = 0; j < subSize; j++)
				{
					outFile.write((char*)&allKeyFrames.keyFrames[i].joints[j], sizeof(float) * 16);
				}
				outFile.write((char*)&allKeyFrames.keyFrames[i].time, sizeof(float));
			}
			//save total duration
			outFile.write((char*)&allKeyFrames.duration, sizeof(float));

			outFile.close();
		}

		delete[] tempName;

		return false;
	}

	bool DxObject::LoadBin(const char* filePath, char* headerChunk, int numberOfBytes)
	{
		

		ifstream inputFile;
		inputFile.open(filePath, ios_base::binary);
		if (inputFile.is_open() && headerChunk != nullptr)
		{
			//Currently ignoring the wireframe and drawwireframe booleans
			//to keep previous choices for demonstration purposes.
			bool tempWireframe = false;
			bool tempDrawWire = false;

			inputFile.read(headerChunk, numberOfBytes);
			inputFile.read((char*)&objectLoaded,	sizeof(objectLoaded));
			inputFile.read((char*)&instanceLoaded,	sizeof(instanceLoaded));
			inputFile.read((char*)&debugMode,		sizeof(debugMode));
			inputFile.read((char*)&tempWireframe,	sizeof(tempWireframe));
			inputFile.read((char*)&tempDrawWire,	sizeof(tempDrawWire));


			//Clear the verts, lets be clear :V
			myVertices.clear();
			unsigned int size = 0;
			float tempX, tempY, tempZ;
			inputFile.read((char*)&size, sizeof(unsigned int));
			for (unsigned int i = 0; i < size; i++)
			{
				inputFile.read((char*)&tempX, sizeof(float));
				inputFile.read((char*)&tempY, sizeof(float));
				inputFile.read((char*)&tempZ, sizeof(float));
				myVertices.push_back(XMFLOAT3(tempX, tempY, tempZ));

			}

			//Clear, lets be clear :V
			size = 0;
			float temp[16] = { 0 };
			inputFile.read((char*)&size, sizeof(unsigned int));
			
			if(size != 0)
				myBindPose.clear();
			
			for (unsigned int i = 0; i < size; i++)
			{
				for (unsigned int x = 0; x < 16; x++)
				{
					inputFile.read((char*)&temp[x], sizeof(float));
				}

				myBindPose.push_back((XMFLOAT4X4)temp) ;

			}

			//Clear, lets be clear :V
			myJoints.clear();
			size = 0;
			inputFile.read((char*)&size, sizeof(unsigned int));
			for (unsigned int i = 0; i < size; i++)
			{
				for (unsigned int x = 0; x < 16; x++)
				{
					inputFile.read((char*)&temp[x], sizeof(float));
				}

				myJoints.push_back((XMFLOAT4X4)temp);

			}

			//Clear, lets be clear :V
			myNormals.clear();
			size = 0;
			float tempW;
			inputFile.read((char*)&size, sizeof(unsigned int));
			for (unsigned int i = 0; i < size; i++)
			{
				inputFile.read((char*)&tempX, sizeof(float));
				inputFile.read((char*)&tempY, sizeof(float));
				inputFile.read((char*)&tempZ, sizeof(float));
				inputFile.read((char*)&tempW, sizeof(float));
				myNormals.push_back(XMFLOAT4(tempX, tempY, tempZ, tempW));

			}

			//Clear, lets be clear :V
			myUVs.clear();
			size = 0;
			inputFile.read((char*)&size, sizeof(unsigned int));
			for (unsigned int i = 0; i < size; i++)
			{
				inputFile.read((char*)&tempX, sizeof(float));
				inputFile.read((char*)&tempY, sizeof(float));
				myUVs.push_back(XMFLOAT2(tempX, tempY));

			}

			myIndices.clear();
			size = 0;
			int tInt = 0;
			inputFile.read((char*)&size, sizeof(unsigned int));
			for (unsigned int i = 0; i < size; i++)
			{
				inputFile.read((char*)&tInt, sizeof(float));
				myIndices.push_back(tInt);

			}

			//Animation clip
			allKeyFrames.keyFrames.clear();
			allKeyFrames.duration = 0;

			size = 0;
			inputFile.read((char*)&size, sizeof(unsigned int));

			unsigned int subSize = 0;
			inputFile.read((char*)&subSize, sizeof(unsigned int));
			oneKeyFrame tempKey;

			for (unsigned int i = 0; i < size; i++)
			{
				for (unsigned int j = 0; j < subSize; j++)
				{
					inputFile.read((char*)&temp[0], sizeof(float) * 16);
					tempKey.joints.push_back(XMFLOAT4X4(temp));
				}
				inputFile.read((char*)&tempX, sizeof(float));
				tempKey.time = tempX;
				allKeyFrames.keyFrames.push_back(tempKey);
				tempKey.joints.clear();
			}
			//save total duration
			inputFile.read((char*)&allKeyFrames.duration, sizeof(float));
			//myNormals.clear();
			//size = 0;
			//float tempW;
			//inputFile.read((char*)&size, sizeof(unsigned int));
			//for (unsigned int i = 0; i < size; i++)
			//{
			//	inputFile.read((char*)&tempX, sizeof(float));
			//	inputFile.read((char*)&tempY, sizeof(float));
			//	inputFile.read((char*)&tempZ, sizeof(float));
			//	inputFile.read((char*)&tempW, sizeof(float));
			//	myNormals.push_back(XMFLOAT4(tempX, tempY, tempZ, tempW));
			//
			//}


			//Test Start
			//myBindPose.clear();
			//myJoints.clear();
			cout << " First Vertex after load: ";
			cout << myVertices[0].x << " " << myVertices[0].y << " " << myVertices[0].z << " " << endl;
			cout << " First Normal after load: ";
			cout << myNormals[0].x << " " << myNormals[0].y << " " << myNormals[0].z << " " << myNormals[0].w << endl;
			cout << " First UV after load: ";
			cout << myUVs[0].x << " " << myUVs[0].y << endl;//Test End
			cout << " First Index after load: ";
			cout << myIndices[0] << endl;//Test End

			inputFile.close();
		}

		//Variable Reset for a fresh load. Might not be required though.


		return false;
	}
};
