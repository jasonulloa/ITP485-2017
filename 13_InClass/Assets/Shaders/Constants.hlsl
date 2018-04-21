// We want to use row major matrices
#pragma pack_matrix(row_major)

// TODO Lab 2f Create Constant Buffers
// TODO Lab 3j Add Camera Position
cbuffer PerCameraConstants : register(b0)
{
	float4x4 c_viewProj;
	float3 c_cameraPosition;
};

cbuffer PerObjectConstants : register(b1)
{
	float4x4 c_modelToWorld;
	float3 c_diffuseColor;
};

// TODO Lab 3d Point Light Data
#define MAX_POINT_LIGHTS 8
struct PointLightData
{
	float3 diffuseColor;
	float3 specularColor;
	float3 position;
	float specularPower;
	float innerRadius;
	float outerRadius;
	bool isEnabled;
};

// TODO Lab 3c Lighting Constants
cbuffer LightingConstants : register(b2)
{
	float3	c_ambient;
	PointLightData c_pointLight[MAX_POINT_LIGHTS];
};

// TODO Lab 2m Texture2D and SamplerState
SamplerState DefaultSampler : register(s0);
Texture2D DiffuseTexture : register(t0);
Texture2D NormalTexture : register(t1);
