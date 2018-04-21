#include "Constants.hlsl"

// Input structs for vertex and pixel shader
struct VS_INPUT
{
	// TODO Lab 3a
	float3 mPos : POSITION;
	float2 mTex : TEXCOORD0;
};

struct PS_INPUT
{
	// TODO Lab 3g
	float4 mPos : SV_POSITION;
	float2 mTex: TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	// TODO Lab 3a
	// (For now, just directly copy position/tex coord)
	output.mPos = float4(input.mPos, 1.0f);
	output.mTex = input.mTex;

	// TODO Lab 3g

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	// TODO Lab 3a
	// TODO Lab 3c ambient light
	// TODO Lab 3g
	// TODO Lab 3h
	// TODO Lab 3j
	return float4(1.0f, 1.0f, 0.0f, 1.0f);    // Yellow, with Alpha = 1
}
