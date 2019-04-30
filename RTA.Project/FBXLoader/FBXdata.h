#pragma once
#include <vector>
#include "../RTA.Project/DXIStructures.h"

//struct animClip;

class FBXdata
{
	public:


		virtual void Terminate() = 0;
		virtual void GetGridVertices(std::vector<DXEnvironment::VertexPositionColor>& myVerts) = 0;
		virtual bool LoadFBX(std::vector<DXEnvironment::VertexPositionColor>& myVerts, std::vector<int>& outIndices, unsigned int &iCount) = 0;
		virtual bool GetModelData() = 0;
		virtual bool LoadJoints(std::vector<XMFLOAT4X4>& transBones, std::vector<int>& lineIndices) = 0;
		//virtual bool LoadAnimations() = 0;
		virtual bool GetAnimationData(std::vector<std::vector<XMFLOAT4X4> >& animClip, std::vector<float>& times, float& duration) = 0;
		virtual void GetTextureName(std::string& name) = 0;


};