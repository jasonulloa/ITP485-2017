// We want to use row major matrices
#pragma pack_matrix(row_major)

cbuffer PerCameraConstants: register(b0) {
	float4x4 gViewProjMatrix;
	float4 gViewProjScale;
	float4 gViewProjOffset;
};

cbuffer PerObjectConstants: register(b1) {
	float4x4 gWorldTransMatrix;
	float4 gWorldTransScale;
	float4 gWorldTransOffset;
}

// TODO Lab 3j Add Camera Position

// TODO Lab 3d Point Light Data

// TODO Lab 3c Lighting Constants

SamplerState DefaultSampler : register(s0);
Texture2D DiffuseTexture : register(t0);