#pragma once

#include <DirectXMath.h>
#include <vector> 
#include <ctime>
#include <d3dcompiler.h>
#pragma comment(lib, "D3DCompiler.lib")
#include "DxInstance.h"

#define TWEEN_T 0.5f
#define TIMER_MODIFIER_VALUE 10

namespace DXEnvironment
{
		
	enum DxObjType { Animation,  Default, Object = 3};
	
	class DxObject
	{

	private:
		DxObjType objectType = Default;
		
		std::clock_t timer;
		bool timerOn = false;
		unsigned short state = 0;
		bool tweenTime = false;
		double timerVal = 0;
		double timerModifier = 1000.0;
		
		//Internal Notifiers
		bool objectLoaded = false;				//Loaded
		bool instanceLoaded = false;			//Loaded
		bool debugMode = false;					//Loaded
		bool wireframeMode = false;				//Loaded
		bool drawWireFrame = true;				//Loaded


		DxInstance* myInstance = nullptr;
		std::vector<XMFLOAT3> myVertices;		//Loaded
		std::vector<XMFLOAT4X4> myBindPose;		//Loaded
		std::vector<XMFLOAT4X4> myJoints;		//Loaded
		std::vector<XMFLOAT4> myNormals;		//Loaded
		std::vector<XMFLOAT2> myUVs;			//Loaded
		std::vector<int> myIndices;				//Loaded

		animClip allKeyFrames;					//Loaded
		short m_currentFrame = 0;				//Reset NO
		TransformStack m_FrameContainer;		//Reset NO
		std::string textureName;				//Loaded

		//Debugging
		unsigned int debugCount = 0;

		Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader = nullptr;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader = nullptr;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout = nullptr;

		//Debug variables
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_mainTexture = nullptr;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_mainSampler = nullptr;


		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_txResView = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_transformBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_boneBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_lineIndexBuffer = nullptr;
		std::vector<VertexPositionColor> debugTransformStack;
		std::vector<VertexPositionColor> debugLineStack;
		std::vector<int> lineIndices;
		unsigned int m_transformPtCount = 0;
		unsigned int m_transformCount = 0;
		unsigned int m_lineIndexCount = 0;

		int attachJoint = -1;
		bool isAttachment = false;
		XMFLOAT4X4 attachTransform;
		//bool customObject = false;

		bool isIndexed = false;
		unsigned int m_indexCount = 0;
		unsigned int m_vertexCount = 0;

		bool isAnimated = false;
		unsigned int m_frameCount = 0;

		VertexPositionColor transformedCube[8] =
		{
			{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
			{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
			{ XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
		};

	public:


		DxObject();
		DxObject(DxObjType myType);
		DxObject(DxInstance& myDX, bool indexed = false);  //Cube Default
		DxObject(DxInstance& myDX, std::vector<VertexPositionColor>& vertices, bool indexed = false, bool debug = false);
		void LoadObject(DxInstance& myDX, bool indexed = false, bool debug = false);
		
		~DxObject();
	
		bool LoadAnimation();
		void LoadVertices(std::vector<XMFLOAT3> vertices) {	myVertices = vertices; }
		bool FullLoad(std::vector<VertexPositionUVNormal> data);
		bool CreateBuffers(bool customObject = false, VertexPositionColor* myPoints = nullptr, unsigned int size = 0, unsigned int* myIndices = nullptr, unsigned int indexSize = 0);
		bool CreateShadersLayout(bool customObject = false);
		
		//Not actual setters, more like loaders.
		bool SetBuffers(bool fullVertex = false);
		bool SetShadersLayout();
		bool DrawObject();
		bool DrawDebugLines();
		bool DrawTransformLines();
		bool TranformObject(XMFLOAT4X4 transform, VertexPositionColor* vertex = nullptr, unsigned int size = 0, bool customObject = false);
		void InitializeDebug();
		bool CreateJCBuffer();
		bool UpdateJCBuffer();


		//Getters
		ID3D11VertexShader* GetVertexShader() { return m_vertexShader.Get(); };
		ID3D11PixelShader* GetPixelShader() { return m_pixelShader.Get(); };
		std::vector<int> GetIndices() const { return myIndices; }
		unsigned int GetIndexCount() { return m_indexCount; }
		animClip* GetAnimClip() { return &allKeyFrames; }

		//inline Setters
		void SetIndices(std::vector<int> indices, unsigned int indexCount) { myIndices = indices; m_indexCount = indexCount; }
		void SetBones(std::vector<XMFLOAT4X4> newBones);
		void SetBoneIndices(std::vector<int> tempIndices) { lineIndices = tempIndices; };
		void SetClip(animClip temp);// { allKeyFrames = temp; isAnimated = true; m_frameCount = allKeyFrames.keyFrames.size(); }
		void SetBindPose(std::vector<XMFLOAT4X4> thePose) { myBindPose = thePose; isAnimated = true; };
		//Updater Methods
		void ProcessInput(unsigned int next);
		void UpdateVertexBuffers();
		void DisableWireframe();

		void InitBufferBones();
		void SwitchJoints(short direction = 0);  
		void SwitchTimedJoints();
		XMFLOAT4X4 CalculateTween(XMFLOAT4X4 preJoints, XMFLOAT4X4 postJoints, float slice = TWEEN_T);
		bool CalculateSkinTransforms();

		bool LoadTexture();
		bool SetTexture();
		void SetTextureName(std::string name) { textureName = name; return;};

		void StartTimer();
		void StopTimer();
		void TimerCheck();

		bool AssignJoint(int joint);
		void SetTransformOffset(XMFLOAT4X4 newJoint);
		bool OffsetObject(VertexPositionColor* myPts);

		bool SaveBin(const char* filePath, const char* headerChunk = NULL, int numberOfBytes = 0);
		bool LoadBin(const char* filePath, char* headerChunk = NULL, int numberOfBytes = 0);
		//Empty :3
	};
};

