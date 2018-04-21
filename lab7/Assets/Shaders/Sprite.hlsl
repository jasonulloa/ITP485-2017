#include "Constants.hlsl"

// Input structs for vertex and pixel shader
struct VS_INPUT
{
	float3 mPos : POSITION;
	float2 mTex : TEXCOORD0;
};

struct PS_INPUT
{
	float4 mPos : SV_POSITION;
	float2 mTex: TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	// TODO Lab 2j Transform the position into screen space
	// (For now, just directly copy position/tex coord)
	output.mPos = mul(float4(input.mPos, 1.0f), c_modelToWorld);
	output.mPos = mul(output.mPos, c_viewProj);
	output.mTex = input.mTex;

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	// TODO Lab 2m get the color from the texture
	float4 diffuse = DiffuseTexture.Sample(DefaultSampler, input.mTex);
	return diffuse;
}
