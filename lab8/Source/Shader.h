// Shader.h
// Shader asset that encapsulates both the vertex
// and fragment shaders

#pragma once
#include "Asset.h"
#include "Math.h"
#include "Texture.h"
#include "GraphicsDriver.h"
#include "PointLightData.h"
#include "MatrixPalette.h"

class Shader : public Asset
{
	DECL_ASSET(Shader, Asset);
public:
	struct PerCameraConstants
	{
		Matrix4 viewProjMatrix;
		Vector3 viewProjScale;
		float unused1 = 0.0f;
		Vector3 viewProjOffset;
		float unused2 = 0.0f;
		Vector3 worldSpaceCamera;
		float exposure;
		float offset;
		Vector3 unused3 = Vector3::Zero;
	};

	struct PerObjectConstants
	{
		Matrix4 worldTransMatrix;
		Vector3 worldTransScale;
		float unused1 = 0.0f;
		Vector3 worldTransOffset;
		float unused2 = 0.0f;
		Vector3 color;
		float unused3 = 0.0f;
	};

	struct LightingConstants
	{
		Vector3 ambientLight;
		float unused = 0.0f;
		PointLightData pointLightArray[8];
	};

	Shader(class Game& game);
	virtual ~Shader();

	void SetActive();

	PerCameraConstants& GetPerCameraConstants() { return mPerCamera; }
	PerObjectConstants& GetPerObjectConstants() { return mPerObject; }
	LightingConstants& GetLightingConstants() { return mLighting; }

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
	MatrixPalette mMatrixPalette;

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
