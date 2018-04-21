// Shader.h
// Shader asset that encapsulates both the vertex
// and fragment shaders

#pragma once
#include "Asset.h"
#include "Math.h"
#include "Texture.h"
#include "GraphicsDriver.h"
#include "PointLightData.h"
#include "Profiler.h"

class Shader : public Asset
{
	DECL_ASSET(Shader, Asset);
public:
	struct PerCameraConstants
	{
		// TODO Lab 2f
		Matrix4 viewProj;
		// TODO Lab 3j
		Vector3 cameraPosition;
		float pad;
	};

	struct PerObjectConstants
	{
		// TODO Lab 2f
		Matrix4 modelToWorld;
		Vector3 diffuseColor;
		float pad;
	};

	struct LightingConstants
	{
		// TODO Lab 3c
		Vector3 ambient;
		float pad;
		// TODO Lab 3d
		PointLightData pointLight[MAX_POINT_LIGHTS];
	};

	Shader(class Game& game);
	virtual ~Shader();

	void SetActive();

	PerCameraConstants& GetPerCameraConstants() { return mPerCamera; }
	PerObjectConstants& GetPerObjectConstants() { return mPerObject; }
	LightingConstants& GetLightingConstants() { PROFILE_SCOPE(Shader_GetLightingConstants); return mLighting; }

	void CreateMatrixPaletteBuffer();

	void UploadPerCameraConstants();
	void UploadPerObjectConstants();
	void UploadLightingConstants();
	void UploadMatrixPalette(const struct MatrixPalette& palette);

	// Bind the specified texture to the specified slot
	void BindTexture(TexturePtr texture, int slot);

	const std::vector<char>& GetCompiledVS() const { return mCompiledVS; }
	const std::vector<char>& GetCompiledPS() const { return mCompiledPS; }
protected:
	bool Load(const char* fileName, class AssetCache* cache) override;
private:
	PerCameraConstants mPerCamera;
	PerObjectConstants mPerObject;
	LightingConstants mLighting;

	GraphicsBufferPtr mPerCameraBuffer;
	GraphicsBufferPtr mPerObjectBuffer;
	GraphicsBufferPtr mLightingBuffer;
	GraphicsBufferPtr mMatrixPaletteBuffer;

	VertexShaderPtr mVertexShader;
	PixelShaderPtr mPixelShader;

	SamplerStatePtr mDefaultSampler;

	std::vector<char> mCompiledVS;
	std::vector<char> mCompiledPS;

	GraphicsDriver& mGraphicsDriver;
};

DECL_PTR(Shader);
