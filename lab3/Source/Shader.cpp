#include "ITPEnginePCH.h"
#include <SDL/SDL_log.h>

Shader::Shader(class Game& game)
	:Asset(game)
	,mGraphicsDriver(mGame.GetRenderer().GetGraphicsDriver())
{
}

Shader::~Shader()
{
}

void Shader::SetActive()
{
	mGraphicsDriver.SetVertexShader(mVertexShader);
	mGraphicsDriver.SetPixelShader(mPixelShader);

	// Set the per-camera and per-object constant buffers for vertex-shader
	mGraphicsDriver.SetVSConstantBuffer(mPerCameraBuffer, 0);
	mGraphicsDriver.SetVSConstantBuffer(mPerObjectBuffer, 1);

	// SetPSSamplerState
	mGraphicsDriver.SetPSSamplerState(mDefaultSampler, 0);

	// Set the per-camera constant buffer for the pixel-shader
	mGraphicsDriver.SetPSConstantBuffer(mPerCameraBuffer, 0);

	// Set the lighting constant buffer
	mGraphicsDriver.SetPSConstantBuffer(mLightingBuffer, 2);

	// TODO Lab 4e
	// Set the matrix palette buffer, if it exists
}

void Shader::CreateMatrixPaletteBuffer()
{
	// TODO Lab 4e
}

void Shader::UploadPerCameraConstants()
{
	void *temp = mGraphicsDriver.MapBuffer(mPerCameraBuffer);
	memcpy(temp, &mPerCamera, sizeof(mPerCamera));
	mGraphicsDriver.UnmapBuffer(mPerCameraBuffer);
}

void Shader::UploadPerObjectConstants()
{
	void *temp = mGraphicsDriver.MapBuffer(mPerObjectBuffer);
	memcpy(temp, &mPerObject, sizeof(mPerObject));
	mGraphicsDriver.UnmapBuffer(mPerObjectBuffer);
}

void Shader::UploadLightingConstants()
{
	void *temp = mGraphicsDriver.MapBuffer(mLightingBuffer);
	memcpy(temp, &mLighting, sizeof(mLighting));
	mGraphicsDriver.UnmapBuffer(mLightingBuffer);
}

void Shader::UploadMatrixPalette(const struct MatrixPalette& palette)
{
	// TODO Lab 4e
}

void Shader::BindTexture(TexturePtr texture, int slot)
{
	texture->SetActive(slot);
}

bool Shader::Load(const char* fileName, class AssetCache* cache)
{
	bool vshaderCompiled = mGraphicsDriver.CompileShaderFromFile(fileName, "VS", "vs_4_0", mCompiledVS);
	bool pshaderCompiled = mGraphicsDriver.CompileShaderFromFile(fileName, "PS", "ps_4_0", mCompiledPS);
	mVertexShader = mGraphicsDriver.CreateVertexShader(mCompiledVS);
	mPixelShader = mGraphicsDriver.CreatePixelShader(mCompiledPS);

	mPerCameraBuffer = mGraphicsDriver.CreateGraphicsBuffer(&mPerCamera, sizeof(mPerCamera), EBF_ConstantBuffer, ECPUAF_CanWrite, EGBU_Dynamic);
	mPerObjectBuffer = mGraphicsDriver.CreateGraphicsBuffer(&mPerObject, sizeof(mPerObject), EBF_ConstantBuffer, ECPUAF_CanWrite, EGBU_Dynamic);
	mLightingBuffer = mGraphicsDriver.CreateGraphicsBuffer(&mLighting, sizeof(mLighting), EBF_ConstantBuffer, ECPUAF_CanWrite, EGBU_Dynamic);

	mDefaultSampler = mGraphicsDriver.CreateSamplerState();

	return vshaderCompiled && pshaderCompiled;
}
