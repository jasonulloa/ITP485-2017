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

	output.mPos = float4(2.0 * input.mPos, 1.0f);
	output.mTex = input.mTex;

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	float offset[3] = { 0.0f, 1.3846153846f/gOffset, 3.2307692308f/gOffset };
	float weight[3] = { 0.2270270270f, 0.3162162162f, 0.0702702703f };

	float4 inPixel = DiffuseTexture.Sample(DefaultSampler, input.mTex) * weight[0];
	for (int i = 1; i < 3; i++) {
		inPixel += DiffuseTexture.Sample(DefaultSampler, (input.mTex + float2(offset[i], 0.0f))) * weight[i];
		inPixel += DiffuseTexture.Sample(DefaultSampler, (input.mTex - float2(offset[i], 0.0f))) * weight[i];
	}

	return inPixel;
}