#pragma once

#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3d11.lib")

#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>

//#include <DirectXPackedVector.h>
//#include <DirectXColors.h>
//#include <DirectXCollision.h>
#include <vector>

//#include ""
#include "stdafx.h"
#include "DXIStructures.h"

//using namespace DirectX;

class FBXdata;

namespace DXEnvironment
{

	class DxInstance
	{ 
		typedef Microsoft::WRL::ComPtr<ID3D11Buffer> Buffer;
		private:
			//std::vector< std::vector<DirectX::XMFLOAT4> > loadedData;
			
			//Controllers
			bool debugMode = false;
			// Registered Data to DirectX
			unsigned int wHeight = 0;
			unsigned int wWidth = 0;
			POINT curPos;
			POINT prevPos;
			HWND wHandle;

			//DLL related objects.
			FBXdata* FBXinstance = nullptr;
			//HINSTANCE fbxDLL = nullptr;

			//DirectX
			CComPtr<ID3D11Device>			d3Device;
			CComPtr<ID3D11DeviceContext>	d3Context;
			CComPtr<ID3D11RenderTargetView> d3RenderView;
			CComPtr<ID3D11DepthStencilView>	d3dDepthStencilView;

			CComPtr<IDXGISwapChain>			d3SwapChain;
			CComPtr<ID3D11InputLayout>		d3InputLayout;
			CComPtr<ID3D11VertexShader>		vertexShader;
			CComPtr<ID3D11PixelShader>		pixelShader;
			CComPtr<ID3D11RasterizerState>	debugRasterizer;
			CComPtr<ID3D11RasterizerState>	defaultRasterizer;

			Buffer							m_mvpBuffer;
			MVPConstantBuffer				m_mvpData;

			//DirectX Pieces
			D3D11_VIEWPORT viewPort;
			D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
			DXGI_SWAP_CHAIN_DESC dxSwapChainDesc;
	
			struct VramSet
			{
				DirectX::XMFLOAT4 constantColor;
				DirectX::XMFLOAT2 constantOffset;
				DirectX::XMFLOAT2 padding;
			};
	
			VramSet shaderSet;
	
			ID3D11Buffer* d3Buffer;

			//Loading variables 
			unsigned int VBStack = 0;			//Vertex Buffer Stack
			unsigned int totalBuffers = 0;		//Total Buffers to load
	
			//Instance Objects and Values
			XTime xTimeInst;
			XMFLOAT4X4 dx_mainCamera;
			//DxObject grid;


			//Example Data
			Buffer	d3ExampleBuffer;
			Buffer	d3ExTriangleBuffer;
			Buffer	d3ExampleBuffer2;
			VramSet toShader;
			VramSet BGtoShader;
	
			double bounceX = 1, bounceY = 1;
			bool shiftedX = false;
			bool shiftedY = false;
			bool decrease = false;
			//unsigned int dxNumVerts = 360;
	
		public:
	
			DxInstance();
			~DxInstance();

			bool InitDX(HWND hWindow, unsigned int width, unsigned int height, bool needCamera);
			bool InitializeCamera(XMFLOAT4X4 &dx_model, XMFLOAT4X4 &dx_view, XMFLOAT4X4 &dx_projection);
			
			HRESULT Refresh();
			void Start();

			bool loadExample();
			bool runExample();

			bool loadCube();
			bool runCube();
	
			bool   loadDLL(char* dll_name, bool isAnimated = true);
			bool unloadDLL();
			bool loadFBX(std::vector<VertexPositionColor>& myvector, std::vector<int>& outIndices, unsigned int &iCount, std::vector<XMFLOAT4X4>& bones, std::vector<int>& lineIndices);
			bool loadAnimations(std::vector<std::vector<XMFLOAT4X4> >& theClips, std::vector<float>& times, float& duration);
			bool runDLL(std::vector<VertexPositionColor>& temp);

			bool CreateMVPBuffer();
			bool UpdateMVPBuffer();

			void MouseInteraction(bool mouseDown = false, bool isExample = false);
			void KeyInteraction(char keyPressed);
			bool run();

			bool EnableGrid();

			//Getters
			ID3D11DeviceContext* GetContext() { return d3Context; }
			ID3D11Device* GetDevice() { return d3Device; }
			bool isDebugMode() { return debugMode; }
			void GetTextureName(std::string& txtName);

			//Setters
			void SetDebug(bool debug);
			//void IncrementBufferCount() {}
	
			
	};
	


}
