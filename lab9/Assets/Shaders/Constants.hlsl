// We want to use row major matrices
#pragma pack_matrix(row_major)

cbuffer PerCameraConstants: register(b0) {
	float4x4 gViewProjMatrix;
	float4 gViewProjScale;
	float4 gViewProjOffset;
	float3 gWorldSpaceCamera;
	float gExposure;
	float gOffset;
};

cbuffer PerObjectConstants: register(b1) {
	float4x4 gWorldTransMatrix;
	float4 gWorldTransScale;
	float4 gWorldTransOffset;
	float3 gColor;
}

struct PointLightData {
	float3 gDiffuseColor;
	float3 gSpecularColor;
	float3 gPosition;
	float gSpecularPower;
	float gInnerRadius;
	float gOuterRadius;
	bool gEnabled;
};

cbuffer LightingConstants: register(b2) {
	float3 gAmbientLight;
	PointLightData gPointLightArray[8];
}

SamplerState DefaultSampler : register(s0);
Texture2D DiffuseTexture : register(t0);
Texture2D NormalMap : register(t1);