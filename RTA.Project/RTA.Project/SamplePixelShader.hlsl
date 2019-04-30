texture2D mainTexture : register(t0);
SamplerState mainSampler : register(s0);

cbuffer MVPConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
	//float4 lightDirection;
	//float4 cameraPos;

};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 uv : UV0;
	float3 norm : TEXCOORD0;
	bool isTxt : TEXTURED;
	float4 viewDir : VIEWDIR;
	float4 worldPos : WORLDPOS;
	//float4 cameraPos : CAMERAPOS;
};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 pixelColor;
	if (input.isTxt)
	{
		pixelColor = mainTexture.Sample(mainSampler, input.uv);
		//float3 intensity, reflection, specular = 0;
		//bool flip = false;
		//
		//float3 tempDirection = 0;
		//float surfaceRatio = 0;
		//float3 coneDir = float3( -1.0f, 0, 0);
		//float spotFactor;
		//float coneRatio = 0.98f;
		//float specularIntensity = 100.0f;
		//float4 lightColor = float4(1.0f, 1.0f, 1.0f, 0);
		//spotFactor = 1.0f;
		//coneDir.z *= -1.0f;
		//
		//tempDirection = normalize(lightDirection.xyz - input.worldPos.xyz);  // Spot Light
		//surfaceRatio = saturate(dot(-tempDirection, coneDir));
		//spotFactor = (surfaceRatio > coneRatio) ? 1.0f : 0;
		//
		//intensity = saturate(dot(tempDirection, input.norm));
		//reflection = normalize(2.0f * intensity*input.norm - tempDirection);
		//specular = pow(saturate(dot(reflection, input.viewDir)), specularIntensity);
		//pixelColor += (float4(intensity, 0.0f)*lightColor)*spotFactor;
		//pixelColor += float4(specular, 0.0f);
	}
	else
	{
		pixelColor = float4(input.uv, 1.0f);
	}


	
	return pixelColor;
}
