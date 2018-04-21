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
	// TODO Lab 2b
	// Set this shader as active
	mGraphicsDriver.SetVertexShader(mVertexShader);
	mGraphicsDriver.SetPixelShader(mPixelShader);

	// TODO Lab 2f
	// Set the per-camera and per-object constant buffers for vertex-shader
	mGraphicsDriver.SetVSConstantBuffer(mPerCameraBuffer, 0);
	mGraphicsDriver.SetVSConstantBuffer(mPerObjectBuffer, 1);

	// TODO Lab 2m
	// SetPSSamplerState
	mGraphicsDriver.SetPSSamplerState(mDefaultSampler, 0);

	// TODO Lab 3j
	// Set the per-camera constant buffer for the pixel-shader
	mGraphicsDriver.SetPSConstantBuffer(mPerCameraBuffer, 0);
	mGraphicsDriver.SetPSConstantBuffer(mPerObjectBuffer, 1);

	// TODO Lab 3c
	// Set the lighting constant buffer
	mGraphicsDriver.SetPSConstantBuffer(mLightingBuffer, 2);

	// TODO Lab 43
	// Set the matrix palette buffer, if it exists
	if (nullptr != mMatrixPaletteBuffer)
		mGraphicsDriver.SetVSConstantBuffer(mMatrixPaletteBuffer, 3);
}

void Shader::CreateMatrixPaletteBuffer()
{
	// TODO Lab 4e
	mMatrixPaletteBuffer = mGraphicsDriver.CreateGraphicsBuffer(nullptr, sizeof(MatrixPalette), EBF_ConstantBuffer, ECPUAF_CanWrite, EGBU_Dynamic);
}

void Shader::UploadPerCameraConstants()
{
	// TODO Lab 2g
	void *mem = mGraphicsDriver.MapBuffer(mPerCameraBuffer);
	memcpy(mem, &mPerCamera, sizeof(mPerCamera));
	mGraphicsDriver.UnmapBuffer(mPerCameraBuffer);
}

void Shader::UploadPerObjectConstants()
{
	// TODO Lab 2g
	void *mem = mGraphicsDriver.MapBuffer(mPerObjectBuffer);
	memcpy(mem, &mPerObject, sizeof(mPerObject));
	mGraphicsDriver.UnmapBuffer(mPerObjectBuffer);
}

void Shader::UploadLightingConstants()
{
	// TODO Lab 3c
	void *mem = mGraphicsDriver.MapBuffer(mLightingBuffer);
	memcpy(mem, &mLighting, sizeof(mLighting));
	mGraphicsDriver.UnmapBuffer(mLightingBuffer);
}

void Shader::UploadMatrixPalette(const struct MatrixPalette& palette)
{
	// TODO Lab 4e
	void *mem = mGraphicsDriver.MapBuffer(mMatrixPaletteBuffer);
	memcpy(mem, &palette, sizeof(palette));
	mGraphicsDriver.UnmapBuffer(mMatrixPaletteBuffer);
}

void Shader::BindTexture(TexturePtr texture, int slot)
{
	// TODO Lab 2m
	texture->SetActive(slot);
}

bool Shader::Load(const char* fileName, class AssetCache* cache)
{
	// TODO Lab 2b load the VS and PS shaders
	if (false == mGraphicsDriver.CompileShaderFromFile(fileName, "VS", "vs_4_0", mCompiledVS))
		return false;
	mVertexShader = mGraphicsDriver.CreateVertexShader(mCompiledVS);

	if (false == mGraphicsDriver.CompileShaderFromFile(fileName, "PS", "ps_4_0", mCompiledPS))
		return false;
	mPixelShader = mGraphicsDriver.CreatePixelShader(mCompiledPS);

	// TODO Lab 2f create constant buffers
	mPerCameraBuffer = mGraphicsDriver.CreateGraphicsBuffer(&mPerCamera, sizeof(mPerCamera), EBF_ConstantBuffer, ECPUAF_CanWrite, EGBU_Dynamic);
	mPerObjectBuffer = mGraphicsDriver.CreateGraphicsBuffer(&mPerObject, sizeof(mPerObject), EBF_ConstantBuffer, ECPUAF_CanWrite, EGBU_Dynamic);
	// TODO Lab 3c create lighting buffer
	mLightingBuffer = mGraphicsDriver.CreateGraphicsBuffer(&mLighting, sizeof(mLighting), EBF_ConstantBuffer, ECPUAF_CanWrite, EGBU_Dynamic);

	// TODO Lab 2m create sampler state
	mDefaultSampler = mGraphicsDriver.CreateSamplerState();

	return true;
}
