#include "Constants.hlsl"

// TODO Lab 4e
#define MAX_SKELETON_BONES 96
cbuffer SkinConstants : register(b3)
{
	float4x4 c_skinMatrix[MAX_SKELETON_BONES];
};

// Input structs for vertex and pixel shader
struct VS_INPUT
{
	// TODO Lab 4b copy from Phong.hlsl
	float3 mPos : POSITION;
	float3 mNorm : NORMAL;
	uint4 mBoneIndex : BONES;
	float4 mWeights : WEIGHTS;
	float2 mTex : TEXCOORD0;
};

struct PS_INPUT
{
	// TODO Lab 4b copy from Phong.hlsl
	float4 mPos : SV_POSITION;
	float2 mTex: TEXCOORD0;
	float3 mWorldPos : POSITION0;
	float3 mWorldNorm : NORMAL0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	// TODO Lab 4b copy from Phong.hlsl
	// TODO Lab 4h skin it
	float4 pos = float4(input.mPos, 1.0);
	input.mPos = mul(pos, c_skinMatrix[input.mBoneIndex.x]) * input.mWeights.x
		+ mul(pos, c_skinMatrix[input.mBoneIndex.y]) * input.mWeights.y
		+ mul(pos, c_skinMatrix[input.mBoneIndex.z]) * input.mWeights.z
		+ mul(pos, c_skinMatrix[input.mBoneIndex.w]) * input.mWeights.w;
	float4 norm = float4(input.mNorm, 0.0);
	input.mNorm = mul(norm, c_skinMatrix[input.mBoneIndex.x]) * input.mWeights.x
		+ mul(norm, c_skinMatrix[input.mBoneIndex.y]) * input.mWeights.y
		+ mul(norm, c_skinMatrix[input.mBoneIndex.z]) * input.mWeights.z
		+ mul(norm, c_skinMatrix[input.mBoneIndex.w]) * input.mWeights.w;

	// (For now, just directly copy position/tex coord)
	output.mPos = mul(float4(input.mPos, 1.0f), c_modelToWorld);
	output.mWorldPos = output.mPos.xyz;
	output.mPos = mul(output.mPos, c_viewProj);
	output.mTex = input.mTex;

	// TODO Lab 3g
	output.mWorldNorm = mul(float4(input.mNorm, 0.0f), c_modelToWorld).xyz;
	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	// TODO Lab 4b copy from Phong.hlsl
	// TODO Lab 3a
	float4 diffuse = DiffuseTexture.Sample(DefaultSampler, input.mTex);
	// TODO Lab 3c ambient light
	float3 lightColor = c_ambient;
	// TODO Lab 3g
	float3 N = normalize(input.mWorldNorm);
	float3 V = normalize(c_cameraPosition - input.mWorldPos);
	for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
	{
		if (c_pointLight[i].isEnabled)
		{
			float3 L = c_pointLight[i].position - input.mWorldPos;
			float dist = length(L);
			if (dist > 0.0)
			{
				L = L / dist;
				float falloff = smoothstep(c_pointLight[i].outerRadius, c_pointLight[i].innerRadius, dist);
				float3 d = falloff * c_pointLight[i].diffuseColor * dot(L, N);
				lightColor += max(0.0, d);

				float3 R = -reflect(L, N);
				float3 s = falloff * c_pointLight[i].specularColor * pow(max(0.0, dot(R, V)), c_pointLight[i].specularPower);
				lightColor += max(0.0, s);
			}
		}
	}
	lightColor = saturate(lightColor);
	// TODO Lab 3h
	// TODO Lab 3j
	return diffuse * float4(lightColor, 1.0); 
}
