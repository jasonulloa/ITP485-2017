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
	float4 inPixel = DiffuseTexture.Sample(DefaultSampler, input.mTex);

	float biggest = max(inPixel.x, max(inPixel.y, inPixel.z));
	if (biggest <= 1.0f) {
		inPixel = float4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	return inPixel;
}