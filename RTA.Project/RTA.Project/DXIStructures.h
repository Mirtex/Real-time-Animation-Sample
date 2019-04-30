#pragma once
#include <DirectXMath.h>
using namespace DirectX;

namespace DXEnvironment
{
	struct SIMPLE_VERTEX
	{
		DirectX::XMFLOAT2 point = { 0,0 };
	};

	struct Vertex
	{
		DirectX::XMFLOAT3 point = { 0,0,0 };
	};

	struct triangle
	{
		DirectX::XMFLOAT2 point1 = { 0,0 };
		DirectX::XMFLOAT2 point2 = { 0,0 };
		DirectX::XMFLOAT2 point3 = { 0,0 };
	};

	struct triangle3D
	{
		DirectX::XMFLOAT3 point1 = { 0,0,0 };
		DirectX::XMFLOAT3 point2 = { 0,0,0 };
		DirectX::XMFLOAT3 point3 = { 0,0,0 };
	};

	__declspec(align(16)) struct MVPConstantBuffer
	{
		XMFLOAT4X4 model;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	};

	__declspec(align(16)) struct TransformStack
	{
		XMFLOAT4X4 transforms[128];
		XMFLOAT4X4 vTransforms[128];
		int numPerFrame = 0;
	};

	// Used to send per-vertex data to the vertex shader.
	__declspec(align(16)) struct VertexPositionColor
	{
		XMFLOAT3 pos;
		XMFLOAT3 color;
		XMFLOAT3 norm;
		XMFLOAT4 weights;
		XMINT4 wJoints;// = { XMINT4(0,0,0,0) };
		int debugIndex = -1;
		int debugBool = 0;
	};

	__declspec(align(16)) struct VertexPositionUVNormal
	{
		XMFLOAT3 pos;
		XMFLOAT3 uv;
		XMFLOAT3 normal;
	};

	struct oneKeyFrame
	{
		float time = 0;
		std::vector<XMFLOAT4X4> joints;
	};

	struct animClip
	{
		float duration;
		std::vector<oneKeyFrame> keyFrames;
	};

	static const VertexPositionColor cubeVertices2[] =
	{
		{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.f,1.5f, 1.0f) },
		{ XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0.f, 0.f, 1.0f) },
		{ XMFLOAT3( 0.5f,  0.5f,  0.5f), XMFLOAT3(1.f, 0.f, 1.0f) },
		{ XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT3(1.f,1.5f, 1.0f) },

		{ XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT3(0.f,1.5f, 1.0f) },
		{ XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT3(0.f, 0.f, 1.0f) },
		{ XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(1.f, 0.f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(1.f,1.5f, 1.0f) },

		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.f,1.5f, 1.0f) },
		{ XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.f, 0.f, 1.0f) },
		{ XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(1.f, 0.f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(1.f,1.5f, 1.0f) },

		{ XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT3(0.f,1.5f, 1.0f) },
		{ XMFLOAT3( 0.5f,  0.5f,  0.5f), XMFLOAT3(0.f, 0.f, 1.0f) },
		{ XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT3(1.f, 0.f, 1.0f) },
		{ XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT3(1.f,1.5f, 1.0f) },

		{ XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0.f,1.5f, 1.0f) },
		{ XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.f, 0.f, 1.0f) },
		{ XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT3(1.f, 0.f, 1.0f) },
		{ XMFLOAT3( 0.5f,  0.5f,  0.5f), XMFLOAT3(1.f,1.5f, 1.0f) },

		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3( 0.f,1.f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3( 0.f,0.f, 1.0f) },
		{ XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT3( 1.f,0.f, 1.0f) },
		{ XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT3( 1.f,1.f, 1.0f) },
	};
	//Not set for Normals
	static const VertexPositionColor cubeVertices[] =
	{
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(	0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(	0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(	0.5f,  0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(	0.5f,  0.5f,  0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
	};

	static const VertexPositionColor debugTransformLines[] = 
	{
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(5.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.8f) },
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(0.0f, 5.0f, 0.0f), XMFLOAT3(0.8f, 1.0f, 0.0f) },
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(0.0f, 0.0f, 5.0f), XMFLOAT3(0.0f, 0.8f, 1.0f) },
	};

	static const VertexPositionColor testTriangle[] =
	{
		{XMFLOAT3(    0,  0.5f, 0),		XMFLOAT3(0.0f, 1.0f, 0.0f)},
		{XMFLOAT3(-1.0f, -1.0f, 0), XMFLOAT3(1.0f, 0.0f, 0.0f)},
		{XMFLOAT3( 1.0f, -1.0f, 0),	XMFLOAT3(0.0f, 0.0f, 1.0f)},
	};
	
	static const unsigned short cubeIndices2[] =
	{

		2, 1, 0, // -z
		3, 2, 0,

		6, 5, 4, // +z
		7, 6, 4,

		10,9, 8,  // -x
		11,10,8,

		14,13,12, // +x
		15,14,12,

		18,17,16, // +y
		19,18,16,

		22,21,20, // -y
		23,22,20,

	};

	static const unsigned int cubeIndices[] =
	{
		0,1,2, // -x
		1,3,2,

		4,6,5, // +x
		5,6,7,

		0,5,1, // -y
		0,4,5,

		2,7,6, // +y
		2,3,7,

		0,6,4, // -z
		0,2,6,

		1,7,3, // +z
		1,5,7,
	};

	static VertexPositionColor gridVerts[] =
	{
		{ XMFLOAT3(-1000.5f, -0.5f, -1000.5f), XMFLOAT3(0.2f, 0.8f, 0.8f) },
		{ XMFLOAT3(-1000.5f, -0.5f,  1000.5f), XMFLOAT3(0.2f, 0.8f, 0.8f) },
		{ XMFLOAT3( 1000.5f, -0.5f,  1000.5f), XMFLOAT3(0.2f, 0.8f, 0.8f) },
		{ XMFLOAT3( 1000.5f, -0.5f, -1000.5f), XMFLOAT3(0.2f, 0.8f, 0.8f) },
	};

	static unsigned int gridIndices[] =
	{
		0,1,2,
		0,2,3,
	};
}
