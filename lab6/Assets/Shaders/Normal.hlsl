#include "Constants.hlsl"

// Input structs for vertex and pixel shader
struct VS_INPUT
{
	float3 mPos : POSITION;
	float3 mNorm : NORMAL0;
	float3 mTan : NORMAL1;
	float2 mTex : TEXCOORD0;
};

struct PS_INPUT
{
	float4 mPos : SV_POSITION;
	float2 mTex : TEXCOORD0;
	float3x3 mTanToWorld : TEXCOORD1;
	float3 worldSpacePosition : WS_POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	// (For now, just directly copy position/tex coord)
	float4 worldTrans = mul(float4(input.mPos, 1.0f), gWorldTransMatrix);
	output.worldSpacePosition = worldTrans.xyz;
	float4 transformed = mul(worldTrans, gViewProjMatrix);
	output.mTanToWorld[0] = mul(input.mTan, gWorldTransMatrix);
	output.mTanToWorld[1] = mul(cross(input.mNorm, input.mTan), gWorldTransMatrix);
	output.mTanToWorld[2] = mul(input.mNorm, gWorldTransMatrix);
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
	float4 normMapTex = NormalMap.Sample(DefaultSampler, input.mTex);
	float3 tanNorm = normMapTex.xyz * 2 - 1;
	tanNorm = mul(tanNorm, input.mTanToWorld);
	tanNorm = normalize(tanNorm);
	float4 phong = ambiLight;
	for (int i = 0; i < 8; i++) {
		if (gPointLightArray[i].gEnabled) {
			float4 lVec = float4(gPointLightArray[i].gPosition, 0.0f) - float4(input.worldSpacePosition, 0.0f);
			lVec = normalize(lVec);
			float dist = distance(gPointLightArray[i].gPosition, input.worldSpacePosition);
			float step = smoothstep(gPointLightArray[i].gOuterRadius, gPointLightArray[i].gInnerRadius, dist);
			phong = phong + step * float4(gPointLightArray[i].gDiffuseColor, 1.0f) * max(dot(float4(tanNorm, 0.0f), lVec), 0.0f);
			float4 rVec = reflect(-lVec, float4(tanNorm, 0.0f));
			float4 vVec = float4(gWorldSpaceCamera, 0.0f) - float4(input.worldSpacePosition, 0.0f);
			vVec = normalize(vVec);
			phong = phong + step * float4(gPointLightArray[i].gSpecularColor, 1.0f) * pow(max(0.0f, dot(rVec, vVec)), gPointLightArray[i].gSpecularPower);
		}
	}
	float4 phongSat = saturate(phong);
	return DiffuseTexture.Sample(DefaultSampler, input.mTex) * phongSat;
}