// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer MVPConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
	//float4 lightDirection;
	//float4 cameraPos;
};

cbuffer TransformBuffer : register(b1)
{
	matrix jointTransform[128];
	matrix vertexTransform[128];
	int perFrame;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos		: POSITION;
	float3 uv		: UV0;
	float3 norm		: NORMAL;
	float4 weights	: WEIGHTS;
	int4 joints		: W_JOINTS;
	int dIndex		: DEBUG_INDEX;	// If Debug, use this index for transform model data.
	int debugBool	: DEBUG_BOOL;	// Is this debugging data?
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 uv : UV;
	float3 norm : TEXCOORD0;
	bool isTxt : TEXTURED0;
	float4 viewDir : VIEWDIR;
	float4 worldPos : WORLDPOS;

};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.pos, 1.0f);
	output.isTxt = false;

	//output.viewDir = mul(input.pos, model);
	//output.viewDir = normalize(cameraPos - output.viewDir);

	// Transform the vertex position into projected space.

	// It's about debug lines. (Bones & transforms)
	if (input.debugBool == 1)
	{
		pos = mul(pos, jointTransform[input.dIndex]);
		pos = mul(pos, model);

	}
	// (It's an animation)
	else if(input.debugBool == 2)
	{
		if (vertexTransform[0][0][0] > 0)
		{
			float4 temp1, temp2, temp3, temp4;
			temp1 = mul(pos, vertexTransform[input.joints.x]);
			temp2 = mul(pos, vertexTransform[input.joints.y]);
			temp3 = mul(pos, vertexTransform[input.joints.z]);
			temp4 = mul(pos, vertexTransform[input.joints.w]);

			pos = temp1*input.weights.x + temp2 *input.weights.y +temp3*input.weights.z
				+ temp4*input.weights.w;

		}
		//pos.x = input.weights.x*temp1.x +
		//		input.weights.y*temp1.x +
		//		input.weights.z*temp1.x +
		//		input.weights.w*temp1.x;
		//
		//pos.y = input.weights.x*temp2.y +
		//		input.weights.y*temp2.y +
		//		input.weights.z*temp2.y +
		//		input.weights.w*temp2.y;
		//
		//pos.z = input.weights.x*temp3.z +
		//		input.weights.y*temp3.z +
		//		input.weights.z*temp3.z +
		//		input.weights.w*temp3.z;
		//
		//pos.w = input.weights.x*temp4.w +
		//		input.weights.y*temp4.w +
		//		input.weights.z*temp4.w +
		//		input.weights.w*temp4.w;


		//pos = mul(pos, vertexTransform[input.joints.x]);
		output.isTxt = true;
		pos = mul(pos, model);
	}
	else if(input.debugBool == 3) // Weapon
	{
		output.isTxt = true;
		pos = mul(pos, jointTransform[input.dIndex]);
		pos = mul(pos, model);

	}
	else //it's something else.
	{
		pos = mul(pos, model);

	}
	output.worldPos = pos; //might need to move before model transformation.

	//output.viewDir = mul(input.pos, model);
	//output.viewDir = normalize(cameraPos - output.viewDir);

	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	// Pass the color through without modification.
	output.uv = input.uv;
	output.norm = input.norm;
	return output;
}

