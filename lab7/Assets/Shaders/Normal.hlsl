#include "Constants.hlsl"

// Input structs for vertex and pixel shader
struct VS_INPUT
{
	// TODO Lab 3a
	float3 mPos : POSITION;
	float3 mNorm : NORMAL0;
	float3 mTan : NORMAL1;
	float2 mTex : TEXCOORD0;
};

struct PS_INPUT
{
	// TODO Lab 3g
	float4 mPos : SV_POSITION;
	float2 mTex: TEXCOORD0;
	float3 mWorldPos : POSITION0;
	float3 mWorldNorm : NORMAL0;
	float3 mWorldTan : NORMAL1;
	float3 mWorldBiTan : NORMAL2;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	// TODO Lab 3a
	output.mPos = mul(float4(input.mPos, 1.0f), c_modelToWorld);
	output.mWorldPos = output.mPos.xyz;
	output.mPos = mul(output.mPos, c_viewProj);
	output.mTex = input.mTex;

	// TODO Lab 3g
	output.mWorldNorm = mul(float4(input.mNorm, 0.0f), c_modelToWorld).xyz;
	output.mWorldTan = mul(float4(input.mTan, 0.0f), c_modelToWorld).xyz;
	output.mWorldBiTan = cross(output.mWorldNorm, output.mWorldTan);
	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	// TODO Lab 3a
	float4 diffuse = DiffuseTexture.Sample(DefaultSampler, input.mTex);
	// TODO Lab 3c ambient light
	float3 lightColor = c_ambient;
	// TODO Lab 3g
	float3 N = normalize(input.mWorldNorm);
	float3 T = normalize(input.mWorldTan);
	float3 B = normalize(input.mWorldBiTan);
	float3x3 TBN = float3x3(T, B, N);
	float3 normal = NormalTexture.Sample(DefaultSampler, input.mTex).xyz;
	normal = 2.0 * normal - 1.0;
	N = mul(normal, TBN);
	N = normalize(N);
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
				lightColor += max(d, 0.0);

				float3 R = -reflect(L, N);
				float3 s = falloff * c_pointLight[i].specularColor * pow(max(0.0, dot(R, V)), c_pointLight[i].specularPower);
				lightColor += max(s, 0.0);
			}
		}
	}
	lightColor = saturate(lightColor);
	// TODO Lab 3h
	// TODO Lab 3j
	return diffuse * float4(lightColor, 1.0);
}
