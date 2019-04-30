
//#pragma comment(lib, "libfbxsdk.lib")

#include "stdafx.h"
#include "DxInstance.h"
//#define FBXSDK_SHARED
//#define PI 3.14159f
#include "Trivial_VS.csh"
#include "Trivial_PS.csh"
#include "SampleVertexShader.csh"
#include "SamplePixelShader.csh"
#include "DxObject.h"

#include "../FBXLoader/FBXdata.h"
#include "../FBXLoader/FBXLoader.h"


#define DLLNAME_SIZE 128
using namespace DirectX;

namespace DXEnvironment
{

	DxInstance::DxInstance()
	{
		//curPos = new POINT();
		GetCursorPos(&curPos);
		ScreenToClient(wHandle, &curPos);
		memset(&dx_mainCamera, 0, sizeof(XMFLOAT4X4));

	}


	DxInstance::~DxInstance()
	{
		unloadDLL();
		//delete VBStack; // delete if dynamic
	}

	//Initialize directx
	bool DxInstance::InitDX(HWND hWindow, unsigned int width, unsigned int height, bool enableDebug)
	{
		wHeight = height;
		wWidth = width;
		wHandle = hWindow;
		ZeroMemory(&dxSwapChainDesc, sizeof(dxSwapChainDesc));

		dxSwapChainDesc.BufferDesc.Width = width;
		dxSwapChainDesc.BufferDesc.Height = height;
		dxSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		dxSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		dxSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		dxSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		dxSwapChainDesc.BufferCount = 1;
		dxSwapChainDesc.OutputWindow = hWindow;
		dxSwapChainDesc.SampleDesc.Count = 1;
		dxSwapChainDesc.SampleDesc.Quality = 0;
		dxSwapChainDesc.Windowed = true;
		dxSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		dxSwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		dxSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		dxSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		unsigned int demFlags = NULL;

#if defined(_DEBUG)
		demFlags = D3D11_CREATE_DEVICE_DEBUG;

#endif // 


		HRESULT test = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, demFlags, &featureLevel, 1, D3D11_SDK_VERSION,
			&dxSwapChainDesc, &d3SwapChain, &d3Device, NULL, &d3Context);

		ID3D11Texture2D *scTexture;
		d3SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&scTexture);
		d3Device->CreateRenderTargetView(scTexture, NULL, &d3RenderView);
		scTexture->Release();
		scTexture = nullptr;

		viewPort.Width = (float)wWidth;
		viewPort.Height = (float)wHeight;
		viewPort.MinDepth = 0.0f;
		viewPort.MaxDepth = 1.0f;
		viewPort.TopLeftX = 0.0f;
		viewPort.TopLeftY = 0.0f;

		//Depth Stencil
		CD3D11_TEXTURE2D_DESC depthStencilDesc(
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			lround(wWidth),
			lround(wHeight),
			1, 
			1, 
			D3D11_BIND_DEPTH_STENCIL
		);

		CComPtr<ID3D11Texture2D> depthStencil;
		d3Device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencil.p);

		CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
		d3Device->CreateDepthStencilView(depthStencil, &depthStencilViewDesc, &d3dDepthStencilView);

		CD3D11_DEFAULT rasterizer_def;
		if (enableDebug)
		{
			debugMode = enableDebug;
			CD3D11_RASTERIZER_DESC rasterizer_desc(rasterizer_def);

			rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
			rasterizer_desc.CullMode = D3D11_CULL_NONE;

			HRESULT res = d3Device->CreateRasterizerState(&rasterizer_desc, &debugRasterizer);
		}

		CD3D11_RASTERIZER_DESC rasterizer_desc(rasterizer_def);
		HRESULT res = d3Device->CreateRasterizerState(&rasterizer_desc, &defaultRasterizer);

		//if (needCamera)
		//{
		//	//InitializeCamera();
		//}

		return true;
	}

	void DxInstance::Start()
	{
		d3Context->OMSetRenderTargets(1, &d3RenderView.p, d3dDepthStencilView);

		viewPort = CD3D11_VIEWPORT(0.0f, 0.0f, static_cast<float>(wWidth), static_cast<float>(wHeight));
		d3Context->RSSetViewports(1, &viewPort);

		float bgColor[] = { 0, 0.5f, 0.5f, 0.5f };
		d3Context->ClearRenderTargetView(d3RenderView, bgColor);
		d3Context->ClearDepthStencilView(d3dDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	}

	void DxInstance::SetDebug(bool isDebug)
	{
		if (isDebug)
		{
			d3Context->RSSetState(debugRasterizer);
		}
		else
		{
			d3Context->RSSetState(defaultRasterizer);
		
		}
	}

	HRESULT DxInstance::Refresh()
	{
		//auto viewport = m_deviceResources->GetScreenViewport();
		d3Context->RSSetViewports(1, &viewPort);
		return d3SwapChain->Present(1, 0);
	}


	bool DxInstance::loadExample()
	{

		unsigned int dxNumVerts = 360;
		SIMPLE_VERTEX circleVerts[360];

		for (unsigned int i = 0; i < dxNumVerts; i++)
		{
			circleVerts[i].point.x = sin(i*XM_PI / 180);
			circleVerts[i].point.y = cos(i*XM_PI / 180);
		}

		for (unsigned int i = 0; i < dxNumVerts; i++)
		{
			circleVerts[i].point.x *= 0.2f;
			circleVerts[i].point.y *= 0.2f;
		}

		D3D11_BUFFER_DESC cBuffer;
		cBuffer.Usage = D3D11_USAGE_IMMUTABLE;
		cBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		cBuffer.CPUAccessFlags = NULL;
		cBuffer.ByteWidth = sizeof(SIMPLE_VERTEX)*dxNumVerts;
		cBuffer.StructureByteStride = sizeof(SIMPLE_VERTEX);
		cBuffer.MiscFlags = NULL;

		D3D11_SUBRESOURCE_DATA srData = { 0 };
		srData.pSysMem = circleVerts;
		unsigned int dxNumTverts = 10 * 20 * 2 * 3;
		unsigned int dxNumTriangles = 10 * 20 * 2;

		d3Device->CreateBuffer(&cBuffer, &srData, d3ExampleBuffer.GetAddressOf());

		D3D11_BUFFER_DESC tBuffer;
		tBuffer.Usage = D3D11_USAGE_IMMUTABLE;
		tBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		tBuffer.CPUAccessFlags = NULL;
		tBuffer.ByteWidth = sizeof(triangle)*dxNumTriangles;
		tBuffer.StructureByteStride = sizeof(triangle);
		tBuffer.MiscFlags = NULL;

		triangle triangleVerts[400];
		float sqOffsetX = 0, sqOffsetY = 0;
		unsigned int triNum = 0;

		for (unsigned int j = 0; j < 20; j++)
		{
			for (unsigned int i = 0; i < 10; i++)
			{
				triangleVerts[0 + triNum].point1.x = -1.0f + sqOffsetX;
				triangleVerts[0 + triNum].point1.y = 0.9f + sqOffsetY;
				triangleVerts[0 + triNum].point2.x = -1.0f + sqOffsetX;
				triangleVerts[0 + triNum].point2.y = 1.0f + sqOffsetY;
				triangleVerts[0 + triNum].point3.x = -0.9f + sqOffsetX;
				triangleVerts[0 + triNum].point3.y = 1.0f + sqOffsetY;
				triangleVerts[1 + triNum].point1.x = -1.0f + sqOffsetX;
				triangleVerts[1 + triNum].point1.y = 0.9f + sqOffsetY;
				triangleVerts[1 + triNum].point2.x = -0.9f + sqOffsetX;
				triangleVerts[1 + triNum].point2.y = 1.0f + sqOffsetY;
				triangleVerts[1 + triNum].point3.x = -0.9f + sqOffsetX;
				triangleVerts[1 + triNum].point3.y = 0.9f + sqOffsetY;
				triNum += 2;
				sqOffsetX += 0.2f;
			}

			if ((j + 1) % 2)
				sqOffsetX = 0.1f;
			else
				sqOffsetX = 0;

			sqOffsetY -= 0.1f;
		}

		D3D11_SUBRESOURCE_DATA trData = { 0 };
		trData.pSysMem = triangleVerts;

		d3Device->CreateBuffer(&tBuffer, &trData, d3ExTriangleBuffer.GetAddressOf());
		d3Device->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), NULL, &vertexShader);
		d3Device->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), NULL, &pixelShader);

		D3D11_INPUT_ELEMENT_DESC layout[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};


		d3Device->CreateInputLayout(layout, 1, Trivial_VS, sizeof(Trivial_VS), &d3InputLayout);

		D3D11_BUFFER_DESC cBuffer2;
		cBuffer2.Usage = D3D11_USAGE_DYNAMIC;
		cBuffer2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cBuffer2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cBuffer2.ByteWidth = sizeof(VramSet);
		cBuffer2.StructureByteStride = sizeof(VramSet);
		cBuffer2.MiscFlags = NULL;

		d3Device->CreateBuffer(&cBuffer2, NULL, d3ExampleBuffer2.GetAddressOf());

		XMFLOAT4 color = { 1.0f, 1.0f, 0, 0 };
		XMFLOAT2 offset = { 0,0 };
		toShader.constantColor = color;
		toShader.constantOffset = offset;

		XMFLOAT4 bgColor = { 0, 0, 0, 0 };
		XMFLOAT2 bgOffset = { 0,0 };
		BGtoShader.constantColor = bgColor;
		BGtoShader.constantOffset = bgOffset;

		return true;
	}

	bool DxInstance::runExample()
	{
		xTimeInst.Signal();
		double deltaTime = xTimeInst.Delta();


		toShader.constantOffset.x += 0; //1.0f*deltaTime*bounceX;	// 0 means circle doesn't move
		toShader.constantOffset.y += 0; //0.5f*deltaTime*bounceY;	// 0 means circle doesn't move
		if ((BGtoShader.constantColor.x >= 0 || BGtoShader.constantColor.y >= 0 || BGtoShader.constantColor.z >= 0) && decrease)
		{
			if (BGtoShader.constantColor.x >= 0)
			{
				BGtoShader.constantColor.x -= 1.0f*deltaTime;
			}
			if (BGtoShader.constantColor.y >= 0)
			{
				BGtoShader.constantColor.y -= 1.0f*deltaTime;
			}
			if (BGtoShader.constantColor.z >= 0)
			{
				BGtoShader.constantColor.z -= 1.0f*deltaTime;
			}

			if (BGtoShader.constantColor.x <= 0 && BGtoShader.constantColor.y <= 0 && BGtoShader.constantColor.z <= 0)
				decrease = false;
		}


		if (toShader.constantOffset.x >= 0.8f || toShader.constantOffset.x <= -0.8f || toShader.constantOffset.y >= 0.8f || toShader.constantOffset.y <= -0.8f)
		{
			if (!shiftedX && toShader.constantOffset.x >= 0.8f)
			{
				shiftedX = true;
				bounceX *= -1;
				BGtoShader.constantColor.x = 0.5f;
			}
			else if (shiftedX && toShader.constantOffset.x <= -0.8f)
			{
				shiftedX = false;
				bounceX *= -1;
				BGtoShader.constantColor.y = 0.5f;
			}
			else if (!shiftedY && toShader.constantOffset.y >= 0.8f)
			{
				shiftedY = true;
				bounceY *= -1;
				BGtoShader.constantColor.z = 0.5f;
				BGtoShader.constantColor.y = 0.5f;
			}
			else if (shiftedY && toShader.constantOffset.y <= -0.8f)
			{
				shiftedY = false;
				bounceY *= -1;
				BGtoShader.constantColor.x = 0.5f;
				BGtoShader.constantColor.y = 0.5f;
			}
			decrease = true;
			xTimeInst.Restart();

		}


		ID3D11RenderTargetView* tempArray[] = { d3RenderView };
		d3Context->OMSetRenderTargets(1, tempArray, NULL);

		d3Context->RSSetViewports(1, &viewPort);


		float color[4] = { 0, 0, 0.5f, 0 };
		d3Context->ClearRenderTargetView(d3RenderView, color);

		unsigned int tempSize = sizeof(XMFLOAT2);
		unsigned int zero = 0;
		auto test = BGtoShader;
		D3D11_MAPPED_SUBRESOURCE mappedSR;
		ZeroMemory(&mappedSR, sizeof(mappedSR));
		d3Context->Map(d3ExampleBuffer2.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSR);
		memcpy(mappedSR.pData, &BGtoShader, sizeof(BGtoShader));

		d3Context->VSSetConstantBuffers(0, 1, d3ExampleBuffer2.GetAddressOf());
		d3Context->IASetVertexBuffers(0, 1, d3ExTriangleBuffer.GetAddressOf(), &tempSize, &zero);
		d3Context->VSSetShader(vertexShader, NULL, NULL);
		d3Context->PSSetShader(pixelShader, NULL, NULL);
		d3Context->IASetInputLayout(d3InputLayout);
		d3Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		d3Context->Draw(1200, 0);


		tempSize = sizeof(SIMPLE_VERTEX);

		ZeroMemory(&mappedSR, sizeof(mappedSR));
		d3Context->Map(d3ExampleBuffer2.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSR);  // comptr is null
		memcpy(mappedSR.pData, &toShader, sizeof(toShader));

		d3Context->VSSetConstantBuffers(0, 1, d3ExampleBuffer2.GetAddressOf());
		d3Context->IASetVertexBuffers(0, 1, d3ExampleBuffer.GetAddressOf(), &tempSize, &zero);

		d3Context->VSSetShader(vertexShader, NULL, NULL);
		d3Context->PSSetShader(pixelShader, NULL, NULL);

		d3Context->IASetInputLayout(d3InputLayout);

		d3Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

		d3Context->Draw(360, 0);

		d3Context->Unmap(d3ExampleBuffer.Get(), NULL);
		d3Context->Unmap(d3ExampleBuffer2.Get(), NULL);


		return true;
	}

	//Not implemented. See DxObject
	bool DxInstance::loadCube()
	{
		//Set up / Load Cube

		//To load the cube from in here, pass the DxObject through, maybe store it for later use.

		return true;
	}

	//Not implemented. See DxObject
	bool DxInstance::runCube()
	{

		//After the DxObject is registered, draw them.
		return true;
	}

	bool DxInstance::CreateMVPBuffer()
	{
		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(MVPConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		if (d3Device->CreateBuffer(&constantBufferDesc, nullptr, &m_mvpBuffer) != S_OK)
			return false;

		//Initialize the mvp.
		InitializeCamera(m_mvpData.model, m_mvpData.view, m_mvpData.projection);
		return true;
	}

	bool DxInstance::UpdateMVPBuffer()
	{
		if (m_mvpBuffer.Get() != nullptr)
		{
			XMStoreFloat4x4(&m_mvpData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&dx_mainCamera))));
			d3Context->UpdateSubresource(m_mvpBuffer.Get(), 0, NULL, &m_mvpData, 0, 0);
			d3Context->VSSetConstantBuffers(0, 1, m_mvpBuffer.GetAddressOf());

			return true;
		}
		return false;
	}

	// Saiph code DLL Implementation
	typedef FBXdata* (*fbxPtr)(); // Pseudotruth cake.
	bool DxInstance::loadDLL(char* dll_name, bool isAnimated)
	{
		FBXinstance = ActivateLoader(dll_name, isAnimated);
	//	unloadDLL();
	//	//static_cast<short>(dll_name)
	//	char charBuffer[DLLNAME_SIZE];
	//	fbxDLL = LoadLibrary(dll_name);
	//
	//	if (!fbxDLL)
	//	{
	//		std::cout << "Error while loading FBX Loader .dll" << std::endl;
	//		system("pause");
	//		return false;
	//	}
	//
	//	fbxPtr temp = reinterpret_cast<fbxPtr>(GetProcAddress(fbxDLL, "ActivateLoader"));
	//	if (!temp)
	//		return false;
	//
	//	FBXinstance = temp();

		return true;
	}

	//Saiph dll deactivation
	bool DxInstance::unloadDLL()
	{
		if (FBXinstance)
		{
			FBXinstance->Terminate();
			FBXinstance = nullptr;
		}
		//if (fbxDLL)
		//{
		//	FreeLibrary(fbxDLL);
		//	fbxDLL = nullptr;
		//}

		return true;
	}

	bool DxInstance::loadFBX(std::vector<VertexPositionColor>& container, std::vector<int>& outIndices, unsigned int &iCount, std::vector<XMFLOAT4X4>& bones, std::vector<int>& lineIndices)
	{
		FBXinstance->GetModelData();
		FBXinstance->LoadFBX(container, outIndices, iCount);
		//Test to see the vertex data
		/*
		for (unsigned int i = 0; i < container.size(); i++)
		{
			if(i % 3 == 0 && i != 0)
				std::cout << std::endl;
		
			std::cout << container[i].pos.x << " " <<container[i].pos.y << " " << container[i].pos.z << std::endl;
			
		}
		*/

		FBXinstance->LoadJoints(bones, lineIndices);
		//FBXinstance->GetTextureName();

		return true;
	}

	bool DxInstance::loadAnimations(std::vector<std::vector<XMFLOAT4X4> >& theClips, std::vector<float>& times, float& duration )
	{

		FBXinstance->GetAnimationData(theClips, times, duration);
		//std::vector<std::vector<XMFLOAT4X4> > theClips;
		//std::vector<float> times;
		//float duration = 1;
		//animClip tempClip;
		//
		//for (unsigned int i = 0; i < (unsigned int)(duration - 1); ++i)
		//{
		//	oneKeyFrame tempFrame;
		//
		//	tempFrame.joints = theClips[i];
		//	tempFrame.time = times[i];
		//
		//	tempClip.keyFrames.push_back(tempFrame);
		//}
		//
		//myClip = tempClip;
		//myClip.duration = duration;

		return true;
	}

	bool DxInstance::runDLL(std::vector<VertexPositionColor>& container)
	{
		FBXinstance->GetGridVertices(container);
		container.size();
		return true;
	}


	bool DxInstance::run()
	{
		return true;
	}

	bool DxInstance::InitializeCamera(XMFLOAT4X4 &dx_model, XMFLOAT4X4 &dx_view, XMFLOAT4X4 &dx_projection)
	{
		//Size outputSize = m_deviceResources->GetOutputSize();
		float aspectRatio = (float)wWidth / (float)wHeight;
		float fovAngleY = 70.0f * XM_PI / 180.0f;

		// This is a simple example of change that can be made when the app is in
		// portrait or snapped view.
		if (aspectRatio < 1.0f)
		{
			fovAngleY *= 2.0f;
		}

		// Note that the OrientationTransform3D matrix is post-multiplied here
		// in order to correctly orient the scene to match the display orientation.
		// This post-multiplication step is required for any draw calls that are
		// made to the swap chain render target. For draw calls to other targets,
		// this transform should not be applied.

		// This sample makes use of a right-handed coordinate system using row-major matrices.
		XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, 0.01f, 1000.0f);

		XMFLOAT4X4 orientation = XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

		XMStoreFloat4x4(&dx_projection, XMMatrixTranspose(perspectiveMatrix * orientationMatrix));

		// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
		static const XMVECTORF32 eye = { 0.0f, 0.7f, -1.5f, 0.0f };
		static const XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
		static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

		XMStoreFloat4x4(&dx_mainCamera, XMMatrixInverse(nullptr, XMMatrixLookAtLH(eye, at, up)));
		XMStoreFloat4x4(&dx_view, XMMatrixTranspose(XMMatrixLookAtLH(eye, at, up)));
		XMStoreFloat4x4(&dx_model, XMMatrixTranspose(XMMatrixRotationY(1)));

		std::cout << dx_model._11 << " " << dx_model._12 << " " << dx_model._13 << " " << dx_model._14 << std::endl;
		std::cout << dx_model._21 << " " << dx_model._22 << " " << dx_model._23 << " " << dx_model._24 << std::endl;
		std::cout << dx_model._31 << " " << dx_model._32 << " " << dx_model._33 << " " << dx_model._34 << std::endl;
		std::cout << dx_model._41 << " " << dx_model._42 << " " << dx_model._43 << " " << dx_model._44 << std::endl;

		return true;
	}

	void DxInstance::MouseInteraction(bool mouseDown, bool isExample)
	{
		prevPos = curPos;

		GetCursorPos(&curPos);
		ScreenToClient(wHandle, &curPos);


		if (mouseDown)
		{
			prevPos = curPos;
			return;
		}

		int dX = (curPos.x - prevPos.x),
			dY = (curPos.y - prevPos.y);

		if (dX == 0 && dY == 0)
			return;

		RECT tempRect;
		GetClientRect(wHandle, &tempRect);
		// Example reaction

		if (isExample)
		{
			toShader.constantOffset.x = ((float)curPos.x / tempRect.right)*2.0f - 1;
			toShader.constantOffset.y = (-(float)curPos.y / tempRect.bottom)*2.0f + 1;
			prevPos = curPos;
			return;
		}
		else//(is a 3D object)
		{
			//UWP Graphics II Implementation
			XMFLOAT4 pos = XMFLOAT4(dx_mainCamera._41, dx_mainCamera._42, dx_mainCamera._43, dx_mainCamera._44);

			dx_mainCamera._41 = 0;
			dx_mainCamera._42 = 0;
			dx_mainCamera._43 = 0;

			XMMATRIX rotX = XMMatrixRotationX(dY * .75f * 0.05f);
			XMMATRIX rotY = XMMatrixRotationY(dX * .75f * 0.05f);

			XMMATRIX temp_camera = XMLoadFloat4x4(&dx_mainCamera);
			temp_camera = XMMatrixMultiply(rotX, temp_camera);
			temp_camera = XMMatrixMultiply(temp_camera, rotY);

			XMStoreFloat4x4(&dx_mainCamera, temp_camera);

			dx_mainCamera._41 = pos.x;
			dx_mainCamera._42 = pos.y;
			dx_mainCamera._43 = pos.z;
		}

		//prevPos = curPos;
	}

	void DxInstance::KeyInteraction(char keyPressed)
	{
		float speed = 5.75f;
		//UWP Graphics II Implementation
		if (keyPressed == 'W')
		{
			XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, speed);
			XMMATRIX temp_camera = XMLoadFloat4x4(&dx_mainCamera);
			XMStoreFloat4x4(&dx_mainCamera, XMMatrixMultiply(translation, temp_camera));
		}
		else if (keyPressed == 'A')
		{
			XMMATRIX translation = XMMatrixTranslation(-speed, 0.0f, 0.0f);
			XMMATRIX temp_camera = XMLoadFloat4x4(&dx_mainCamera);
			XMStoreFloat4x4(&dx_mainCamera, XMMatrixMultiply(translation, temp_camera));

		}
		else if (keyPressed == 'S')
		{
			XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, -speed);
			XMMATRIX temp_camera = XMLoadFloat4x4(&dx_mainCamera);
			XMStoreFloat4x4(&dx_mainCamera, XMMatrixMultiply(translation, temp_camera));
		}
		else if (keyPressed == 'D')
		{
			XMMATRIX translation = XMMatrixTranslation(speed, 0.0f, 0.0f);
			XMMATRIX temp_camera = XMLoadFloat4x4(&dx_mainCamera);
			XMStoreFloat4x4(&dx_mainCamera, XMMatrixMultiply(translation, temp_camera));
		}
		else if (keyPressed == 'V')
		{
			XMMATRIX translation = XMMatrixTranslation(0.0f, -speed, 0.0f);
			XMMATRIX temp_camera = XMLoadFloat4x4(&dx_mainCamera);
			XMStoreFloat4x4(&dx_mainCamera, XMMatrixMultiply(translation, temp_camera));
		}
		else if (keyPressed == VK_SPACE)
		{
			XMMATRIX translation = XMMatrixTranslation(0.0f, speed, 0.0f);
			XMMATRIX temp_camera = XMLoadFloat4x4(&dx_mainCamera);
			XMStoreFloat4x4(&dx_mainCamera, XMMatrixMultiply(translation, temp_camera));
		}


	}

	bool DxInstance::EnableGrid()
	{
		//Due to include loops, internal grid is postponed.
		return true;
	}

	void DxInstance::GetTextureName(std::string& txtName) 
	{ 
		FBXinstance->GetTextureName(txtName);
	};

}