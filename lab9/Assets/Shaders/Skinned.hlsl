#include "Constants.hlsl"

cbuffer MatrixPalette: register(b3) {
	float4x4 gPalette[96];
}

// Input structs for vertex and pixel shader
struct VS_INPUT
{
	float3 mPos : POSITION;
	float3 mNorm : NORMAL;
	uint4 mBones : BONES;
	float4 mWeights : WEIGHTS;
	float2 mTex : TEXCOORD0;
};

struct PS_INPUT
{
	float4 mPos : SV_POSITION;
	float2 mTex : TEXCOORD0;
	float3 worldSpaceNormal : WS_NORMAL;
	float3 worldSpacePosition : WS_POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	float4 skinPos = input.mWeights.x * mul(float4(input.mPos, 1.0f), gPalette[input.mBones.x]);
	skinPos += input.mWeights.y * mul(float4(input.mPos, 1.0f), gPalette[input.mBones.y]);
	skinPos += input.mWeights.z * mul(float4(input.mPos, 1.0f), gPalette[input.mBones.z]);
	skinPos += input.mWeights.w * mul(float4(input.mPos, 1.0f), gPalette[input.mBones.w]);

	float4 skinNorm = input.mWeights.x * mul(float4(input.mNorm, 0.0f), gPalette[input.mBones.x]);
	skinNorm += input.mWeights.y * mul(float4(input.mNorm, 0.0f), gPalette[input.mBones.y]);
	skinNorm += input.mWeights.z * mul(float4(input.mNorm, 0.0f), gPalette[input.mBones.z]);
	skinNorm += input.mWeights.w * mul(float4(input.mNorm, 0.0f), gPalette[input.mBones.w]);

	float4 worldTrans = mul(skinPos, gWorldTransMatrix);
	output.worldSpacePosition = worldTrans.xyz;
	float4 normTrans = mul(skinNorm, gWorldTransMatrix);
	output.worldSpaceNormal = normTrans.xyz;
	float4 transformed = mul(worldTrans, gViewProjMatrix);
	output.mPos = transformed;
	output.mTex = input.mTex;

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	float4 ambiLight = float4(gAmbientLight, 1.0f);
	input.worldSpaceNormal = normalize(input.worldSpaceNormal);
	float4 phong = ambiLight;
	for (int i = 0; i < 8; i++) {
		if (gPointLightArray[i].gEnabled) {
			float4 lVec = float4(gPointLightArray[i].gPosition, 0.0f) - float4(input.worldSpacePosition, 0.0f);
			lVec = normalize(lVec);
			float dist = distance(gPointLightArray[i].gPosition, input.worldSpacePosition);
			float step = smoothstep(gPointLightArray[i].gOuterRadius, gPointLightArray[i].gInnerRadius, dist);
			phong = phong + step * float4(gPointLightArray[i].gDiffuseColor, 1.0f) * max(dot(float4(input.worldSpaceNormal, 0.0f), lVec), 0.0f);
			float4 rVec = reflect(-lVec, float4(input.worldSpaceNormal, 0.0f));
			float4 vVec = float4(gWorldSpaceCamera, 0.0f) - float4(input.worldSpacePosition, 0.0f);
			vVec = normalize(vVec);
			phong = phong + step * float4(gPointLightArray[i].gSpecularColor, 1.0f) * pow(max(0.0f, dot(rVec, vVec)), gPointLightArray[i].gSpecularPower);
		}
	}
	return DiffuseTexture.Sample(DefaultSampler, input.mTex) * phong;
}
