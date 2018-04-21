#include "ITPEnginePCH.h"

Renderer::Renderer(Game& game)
	:mGame(game)
	,mWindow(nullptr)
	,mWidth(0)
	,mHeight(0)
{

}

Renderer::~Renderer()
{
	// Clear components...
	mDrawComponents.clear();
	mComponents2D.clear();

	mDepthBuffer.reset();
	mSpriteDepthState.reset();
	mMeshDepthState.reset();
	mRasterState.reset();

	mSpriteBlendState.reset();
	mMeshBlendState.reset();

	mSpriteShader.reset();
	mSpriteVerts.reset();

	mMeshShaders.clear();

	// Shutdown the input cache and graphics driver
	mInputLayoutCache.reset();
	mGraphicsDriver.reset();

	if (mWindow != nullptr)
	{
		SDL_DestroyWindow(mWindow);
	}
}

bool Renderer::Init(int width, int height)
{
	// Create our SDL window
	mWindow = SDL_CreateWindow("ITP Engine 2 Demo!", 100, 100, width, height, 
		0);

	if (!mWindow)
	{
		SDL_Log("Could not create window.");
		return false;
	}

	mGraphicsDriver = std::make_shared<GraphicsDriver>(GetActiveWindow());
	mInputLayoutCache = std::make_shared<InputLayoutCache>();

	mWidth = width;
	mHeight = height;
	mExposure = 0.1f;

	if (!InitFrameBuffer())
	{
		return false;
	}

	if (!InitShaders())
	{
		return false;
	}

	if (!InitSpriteVerts())
	{
		return false;
	}

	MakeRenderTargets();

	return true;
}

void Renderer::MakeRenderTargets() {
	GraphicsTexturePtr offscreenTexture1;
	GraphicsTexturePtr offscreenTexture2;
	GraphicsTexturePtr halfTexture;
	GraphicsTexturePtr quarterTexture1;
	GraphicsTexturePtr quarterTexture2;
	mGraphicsDriver->SetOffscreenRenderTarget1(mGraphicsDriver->CreateOffscreenRenderTarget(mGraphicsDriver->CreateOffscreenTexture(offscreenTexture1, mWidth, mHeight)));
	mGraphicsDriver->SetOffscreenRenderTarget2(mGraphicsDriver->CreateOffscreenRenderTarget(mGraphicsDriver->CreateOffscreenTexture(offscreenTexture2, mWidth, mHeight)));
	mGraphicsDriver->SetHalfRenderTarget(mGraphicsDriver->CreateOffscreenRenderTarget(mGraphicsDriver->CreateOffscreenTexture(halfTexture, mWidth/2, mHeight/2)));
	mGraphicsDriver->SetQuarterRenderTarget1(mGraphicsDriver->CreateOffscreenRenderTarget(mGraphicsDriver->CreateOffscreenTexture(quarterTexture1, mWidth/4, mHeight/4)));
	mGraphicsDriver->SetQuarterRenderTarget2(mGraphicsDriver->CreateOffscreenRenderTarget(mGraphicsDriver->CreateOffscreenTexture(quarterTexture2, mWidth/4, mHeight/4)));
	mOffscreenTexture1 = std::make_shared<Texture>(this->mGame, offscreenTexture1, mWidth, mHeight);
	mOffscreenTexture2 = std::make_shared<Texture>(this->mGame, offscreenTexture2, mWidth, mHeight);
	mHalfTexture = std::make_shared<Texture>(this->mGame, halfTexture, mWidth/2, mHeight/2);
	mQuarterTexture1 = std::make_shared<Texture>(this->mGame, quarterTexture1, mWidth/4, mHeight/4);
	mQuarterTexture2 = std::make_shared<Texture>(this->mGame, quarterTexture2, mWidth/4, mHeight/4);
}

void Renderer::RenderFrame()
{
	//Draw the scene
	mGraphicsDriver->SetDepthStencilState(mSpriteDepthState);
	mGraphicsDriver->SetDepthStencil(mDepthBuffer);
	mGraphicsDriver->SetBlendState(mSpriteBlendState);
	mGraphicsDriver->SetRenderTarget(mGraphicsDriver->GetOffscreenRenderTarget1());
	Clear();
	DrawComponents();

	//Begin post-effects
	Matrix4 trans = Matrix4::CreateScale(500.0f);
	DepthStencilStatePtr disableState = mGraphicsDriver->CreateDepthStencilState(false, ECF_Never);
	mGraphicsDriver->SetDepthStencilState(disableState);
	mGraphicsDriver->SetDepthStencil(nullptr);
	mGraphicsDriver->SetBlendState(mMeshBlendState);

	//Draw the HDR (exposure)
	mGraphicsDriver->SetRenderTarget(mGraphicsDriver->GetOffscreenRenderTarget2());
	DrawHDRSprite(mOffscreenTexture1, 1.0f, trans);
	
	//Reduce the image to 1/2 size
	mGraphicsDriver->SetRenderTarget(mGraphicsDriver->GetHalfRenderTarget());
	mGraphicsDriver->SetViewport(0.0f, 0.0f, mWidth / 2, mHeight / 2);
	DrawBloomMaskSprite(mOffscreenTexture2, trans);
	
	//Reduce the image to 1/4 size
	mGraphicsDriver->SetRenderTarget(mGraphicsDriver->GetQuarterRenderTarget1());
	mGraphicsDriver->SetViewport(0.0f, 0.0f, mWidth / 4, mHeight / 4);
	DrawHDRSprite(mHalfTexture, 1.0f, trans);

	//Perform Horizontal Gaussian blur
	mGraphicsDriver->SetRenderTarget(mGraphicsDriver->GetQuarterRenderTarget2());
	DrawHorizontalBlurSprite(mQuarterTexture1, mQuarterTexture1->GetWidth(), trans);

	//Make sure nothing is still bound as a texture
	GraphicsTexturePtr none;
	mGraphicsDriver->SetPSTexture(none, 0);

	//Perform Vertical Gaussian blur
	mGraphicsDriver->SetRenderTarget(mGraphicsDriver->GetQuarterRenderTarget1());
	DrawVerticalBlurSprite(mQuarterTexture2, mQuarterTexture1->GetHeight(), trans);

	//Copy full-size HDR output to the back buffer
	mGraphicsDriver->SetRenderTarget(mGraphicsDriver->GetBackBufferRenderTarget());
	mGraphicsDriver->SetViewport(0.0f, 0.0f, mWidth, mHeight);
	DrawHDRSprite(mOffscreenTexture2, 1.0f, trans);

	//Enable additive blend mode
	mGraphicsDriver->SetRenderTarget(mGraphicsDriver->GetBackBufferRenderTarget());
	mGraphicsDriver->SetBlendState(mAdditiveBlendState);
	DrawHDRSprite(mQuarterTexture1, 1.0f, trans);

	//Again, make sure nothing is still bound as a texture
	mGraphicsDriver->SetPSTexture(none, 0);

	//Present the final image
	Present();
}

void Renderer::AddComponent(DrawComponentPtr component)
{
	if (IsA<SpriteComponent>(component) || IsA<FontComponent>(component))
	{
		mComponents2D.emplace(component);
	}
	else
	{
		mDrawComponents.emplace(component);
	}
}

void Renderer::RemoveComponent(DrawComponentPtr component)
{
	if (IsA<SpriteComponent>(component) || IsA<FontComponent>(component))
	{
		auto iter = mComponents2D.find(component);
		if (iter != mComponents2D.end())
		{
			mComponents2D.erase(iter);
		}
	}
	else
	{
		auto iter = mDrawComponents.find(component);
		if (iter != mDrawComponents.end())
		{
			mDrawComponents.erase(iter);
		}
	}
}

void Renderer::AddPointLight(PointLightComponentPtr light)
{
	mPointLights.emplace(light);
}

void Renderer::RemovePointLight(PointLightComponentPtr light)
{
	mPointLights.erase(light);
}

void Renderer::UpdatePointLights()
{
	for (auto shader : mMeshShaders) {
		int numLights = 0;
		for (auto light : mPointLights) {
			shader.second->GetLightingConstants().pointLightArray[numLights] = light->mData;
			numLights++;
		}
		while (numLights < 8) {
			shader.second->GetLightingConstants().pointLightArray[numLights].enabled = false;
			numLights++;
		}
		shader.second->UploadLightingConstants();
	}
}

void Renderer::DrawSprite(TexturePtr texture, const Matrix4& worldTransform)
{
	mSpriteShader->SetActive();

	mSpriteShader->GetPerObjectConstants().worldTransMatrix = worldTransform;
	mSpriteShader->UploadPerObjectConstants();

	mSpriteShader->BindTexture(texture, 0);

	DrawVertexArray(mSpriteVerts);
}

void Renderer::DrawHDRSprite(TexturePtr texture, float exposure, const Matrix4& worldTransform)
{
	mHDRShader->SetActive();

	mHDRShader->GetPerObjectConstants().worldTransMatrix = worldTransform;
	mHDRShader->UploadPerObjectConstants();

	mHDRShader->GetPerCameraConstants().exposure = exposure;
	mHDRShader->UploadPerCameraConstants();

	mHDRShader->BindTexture(texture, 0);

	DrawVertexArray(mSpriteVerts);
}

void Renderer::DrawBloomMaskSprite(TexturePtr texture, const Matrix4& worldTransform)
{
	mBloomMaskShader->SetActive();

	mBloomMaskShader->GetPerObjectConstants().worldTransMatrix = worldTransform;
	mBloomMaskShader->UploadPerObjectConstants();

	mBloomMaskShader->BindTexture(texture, 0);

	DrawVertexArray(mSpriteVerts);
}

void Renderer::DrawHorizontalBlurSprite(TexturePtr texture, float width, const Matrix4& worldTransform)
{
	mHPassShader->SetActive();

	mHPassShader->GetPerObjectConstants().worldTransMatrix = worldTransform;
	mHPassShader->GetPerCameraConstants().offset = width;
	mHPassShader->UploadPerObjectConstants();
	mHPassShader->UploadPerCameraConstants();

	mHPassShader->BindTexture(texture, 0);

	DrawVertexArray(mSpriteVerts);
}

void Renderer::DrawVerticalBlurSprite(TexturePtr texture, float height, const Matrix4& worldTransform)
{
	mVPassShader->SetActive();

	mVPassShader->GetPerObjectConstants().worldTransMatrix = worldTransform;
	mVPassShader->GetPerCameraConstants().offset = height;
	mVPassShader->UploadPerObjectConstants();
	mVPassShader->UploadPerCameraConstants();

	mVPassShader->BindTexture(texture, 0);

	DrawVertexArray(mSpriteVerts);
}

void Renderer::DrawMesh(VertexArrayPtr vertArray, std::vector<TexturePtr> texture, const Matrix4& worldTransform, Vector3 color, EMeshShader type)
{
	int count = 0;

	mMeshShaders.at(type)->SetActive();

	mMeshShaders.at(type)->GetPerObjectConstants().worldTransMatrix = worldTransform;
	mMeshShaders.at(type)->GetPerObjectConstants().color = color;
	mMeshShaders.at(type)->UploadPerObjectConstants();
	mMeshShaders.at(type)->UploadLightingConstants();

	for (auto text : texture) {
		mMeshShaders.at(type)->BindTexture(text, count);
		count++;
	}

	DrawVertexArray(vertArray);
}

void Renderer::DrawSkeletalMesh(VertexArrayPtr vertArray, TexturePtr texture, const Matrix4& worldTransform, const struct MatrixPalette& palette)
{
	mMeshShaders.at(EMS_Skinned)->SetActive();

	mMeshShaders.at(EMS_Skinned)->GetPerObjectConstants().worldTransMatrix = worldTransform;
	mMeshShaders.at(EMS_Skinned)->UploadPerObjectConstants();
	mMeshShaders.at(EMS_Skinned)->UploadLightingConstants();

	mMeshShaders.at(EMS_Skinned)->BindTexture(texture, 0);
	mMeshShaders.at(EMS_Skinned)->UploadMatrixPalette(palette);

	DrawVertexArray(vertArray);
}

void Renderer::DrawVertexArray(VertexArrayPtr vertArray)
{
	vertArray->SetActive();
	mGraphicsDriver->DrawIndexed(vertArray->GetIndexCount(), 0, 0);
}

void Renderer::UpdateViewMatrix(const Matrix4& view)
{
	mView = view;
	Matrix4 invView = view;
	invView.Invert();
	Vector3 camPos = invView.GetTranslation();
	for (auto shader : mMeshShaders) {
		shader.second->GetPerCameraConstants().viewProjMatrix = mView * mProj;
		shader.second->GetPerCameraConstants().worldSpaceCamera = camPos;
	}
}

void Renderer::SetAmbientLight(const Vector3& color)
{
	for (auto shader : mMeshShaders) {
		shader.second->GetLightingConstants().ambientLight = color;
		shader.second->UploadLightingConstants();
	}
}

Vector3 Renderer::Unproject(const Vector3& screenPoint) const
{
	// Convert screenPoint to device coordinates (between -1 and +1)
	Vector3 deviceCoord = screenPoint;
	deviceCoord.x /= (mWidth) * 0.5f;
	deviceCoord.y /= (mHeight) * 0.5f;

	// First, undo the projection
	Matrix4 unprojection = mProj;
	unprojection.Invert();
	Vector3 unprojVec = TransformWithPerspDiv(deviceCoord, unprojection);

	// Now undo the view matrix
	Matrix4 uncamera = mView;
	uncamera.Invert();
	return Transform(unprojVec, uncamera);
}

void Renderer::Clear()
{
	mGraphicsDriver->ClearRenderTarget(mGraphicsDriver->GetCurrentRenderTarget(), Vector3::Zero, 1);
	mGraphicsDriver->ClearBackBuffer(Vector3::Zero, 1.0f);
	mGraphicsDriver->ClearDepthStencil(mDepthBuffer, 1.0f);
}

void Renderer::DrawComponents()
{
	// Enable depth buffering
	mGraphicsDriver->SetDepthStencilState(mMeshDepthState);

	// Disable blending
	mGraphicsDriver->SetBlendState(mMeshBlendState);

	// Upload per camera constants
	for (auto shader : mMeshShaders) {
		shader.second->UploadPerCameraConstants();
	}
	
	// Update point lights
	UpdatePointLights();

	// Draw the normal components
	for (auto& comp : mDrawComponents)
	{
		if (comp->IsVisible())
		{
			comp->Draw(*this);
		}
	}

	// Disable depth buffering
	mGraphicsDriver->SetDepthStencilState(mSpriteDepthState);

	// Enable blending
	mGraphicsDriver->SetBlendState(mSpriteBlendState);

	// Draw the 2D components
	for (auto& comp : mComponents2D)
	{
		if (comp->IsVisible())
		{
			comp->Draw(*this);
		}
	}
}

void Renderer::Present()
{
	mGraphicsDriver->Present();
}

bool Renderer::InitFrameBuffer()
{
	mRasterState = mGraphicsDriver->CreateRasterizerState(EFM_Solid);
	mGraphicsDriver->SetRasterizerState(mRasterState);
	
	mDepthBuffer = mGraphicsDriver->CreateDepthStencil(mWidth, mHeight);
	mGraphicsDriver->SetDepthStencil(mDepthBuffer);
	mMeshDepthState = mGraphicsDriver->CreateDepthStencilState(true, ECF_LessEqual);
	mSpriteDepthState = mGraphicsDriver->CreateDepthStencilState(false, ECF_Always);

	mSpriteBlendState = mGraphicsDriver->CreateBlendState(true);
	mMeshBlendState = mGraphicsDriver->CreateBlendState(false);
	mAdditiveBlendState = mGraphicsDriver->CreateAdditiveBlendState();

	return true;
}

bool Renderer::InitShaders()
{
	mSpriteShader = mGame.GetAssetCache().Load<Shader>("Shaders/Sprite.hlsl");

	if (mSpriteShader == nullptr) {
		return false;
	}

	InputLayoutElement elements[] = {
		InputLayoutElement("POSITION", 0, EGF_R32G32B32_Float, 0),
		InputLayoutElement("TEXCOORD", 0, EGF_R32G32_Float, 12),
	};

	InputLayoutPtr inputLayout = mGraphicsDriver->CreateInputLayout(elements, 2, mSpriteShader->GetCompiledVS());
	mInputLayoutCache->RegisterLayout("positiontexcoord", inputLayout);

	// Set up the orthographic view matrix
	mSpriteShader->GetPerCameraConstants().viewProjMatrix = Matrix4::CreateOrtho(static_cast<float>(mWidth), static_cast<float>(mHeight), 1000.0f, -1000.0f);
	mSpriteShader->GetPerCameraConstants().offset = 1.0f;
	mSpriteShader->GetPerCameraConstants().exposure = 1.0f;
	mSpriteShader->UploadPerCameraConstants();

	// Set up the projection matrix
	mProj = Matrix4::CreatePerspectiveFOV(Math::ToRadians(70.0f), static_cast<float>(mWidth), static_cast<float>(mHeight), 25.0f, 10000.0f);

	// Load HDR shader and create appropriate input layout
	mHDRShader = mGame.GetAssetCache().Load<Shader>("Shaders/HDR.hlsl");

	if (mHDRShader == nullptr) {
		return false;
	}

	InputLayoutElement offscreenElements[] = {
		InputLayoutElement("POSITION", 0, EGF_R32G32B32_Float, 0),
		InputLayoutElement("TEXCOORD", 0, EGF_R32G32_Float, 12),
	};

	InputLayoutPtr inputOffscreenLayout = mGraphicsDriver->CreateInputLayout(offscreenElements, 2, mHDRShader->GetCompiledVS());
	mInputLayoutCache->RegisterLayout("positiontexcoord", inputOffscreenLayout);

	// Set up the orthographic view matrix
	mHDRShader->GetPerCameraConstants().viewProjMatrix = Matrix4::CreateOrtho(static_cast<float>(mWidth), static_cast<float>(mHeight), 1000.0f, -1000.0f);
	mHDRShader->GetPerCameraConstants().offset = 1.0f;
	mHDRShader->GetPerCameraConstants().exposure = mExposure;
	mHDRShader->UploadPerCameraConstants();

	// Load BloomMask shader and create appropriate input layout
	mBloomMaskShader = mGame.GetAssetCache().Load<Shader>("Shaders/BloomMask.hlsl");

	if (mBloomMaskShader == nullptr) {
		return false;
	}

	InputLayoutElement bloomMaskElements[] = {
		InputLayoutElement("POSITION", 0, EGF_R32G32B32_Float, 0),
		InputLayoutElement("TEXCOORD", 0, EGF_R32G32_Float, 12),
	};

	InputLayoutPtr inputBloomMaskLayout = mGraphicsDriver->CreateInputLayout(bloomMaskElements, 2, mBloomMaskShader->GetCompiledVS());
	mInputLayoutCache->RegisterLayout("positiontexcoord", inputBloomMaskLayout);

	// Set up the orthographic view matrix
	mBloomMaskShader->GetPerCameraConstants().viewProjMatrix = Matrix4::CreateOrtho(static_cast<float>(mWidth), static_cast<float>(mHeight), 1000.0f, -1000.0f);
	mBloomMaskShader->GetPerCameraConstants().offset = 1.0f;
	mBloomMaskShader->GetPerCameraConstants().exposure = 1.0f;
	mBloomMaskShader->UploadPerCameraConstants();

	// Load HPass shader and create appropriate input layout
	mHPassShader = mGame.GetAssetCache().Load<Shader>("Shaders/HPass.hlsl");

	if (mHPassShader == nullptr) {
		return false;
	}

	InputLayoutElement hPassElements[] = {
		InputLayoutElement("POSITION", 0, EGF_R32G32B32_Float, 0),
		InputLayoutElement("TEXCOORD", 0, EGF_R32G32_Float, 12),
	};

	InputLayoutPtr inputHPassLayout = mGraphicsDriver->CreateInputLayout(hPassElements, 2, mHPassShader->GetCompiledVS());
	mInputLayoutCache->RegisterLayout("positiontexcoord", inputHPassLayout);

	// Set up the orthographic view matrix
	mHPassShader->GetPerCameraConstants().viewProjMatrix = Matrix4::CreateOrtho(static_cast<float>(mWidth), static_cast<float>(mHeight), 1000.0f, -1000.0f);
	mHPassShader->GetPerCameraConstants().offset = 1.0f;
	mHPassShader->GetPerCameraConstants().exposure = 1.0f;
	mHPassShader->UploadPerCameraConstants();

	// Load VPass shader and create appropriate input layout
	mVPassShader = mGame.GetAssetCache().Load<Shader>("Shaders/VPass.hlsl");

	if (mVPassShader == nullptr) {
		return false;
	}

	InputLayoutElement vPassElements[] = {
		InputLayoutElement("POSITION", 0, EGF_R32G32B32_Float, 0),
		InputLayoutElement("TEXCOORD", 0, EGF_R32G32_Float, 12),
	};

	InputLayoutPtr inputVPassLayout = mGraphicsDriver->CreateInputLayout(vPassElements, 2, mVPassShader->GetCompiledVS());
	mInputLayoutCache->RegisterLayout("positiontexcoord", inputVPassLayout);

	// Set up the orthographic view matrix
	mVPassShader->GetPerCameraConstants().viewProjMatrix = Matrix4::CreateOrtho(static_cast<float>(mWidth), static_cast<float>(mHeight), 1000.0f, -1000.0f);
	mVPassShader->GetPerCameraConstants().offset = 1.0f;
	mVPassShader->GetPerCameraConstants().exposure = 1.0f;
	mVPassShader->UploadPerCameraConstants();

	// Load and set up the BasicMesh shader and create appropriate input layout
	{
		mMeshShaders.emplace(EMS_Basic, mGame.GetAssetCache().Load<Shader>("Shaders/BasicMesh.hlsl"));

		InputLayoutElement meshElements[] = {
			InputLayoutElement("POSITION", 0, EGF_R32G32B32_Float, 0),
			InputLayoutElement("NORMAL", 0, EGF_R32G32B32_Float, 12),
			InputLayoutElement("TEXCOORD", 0, EGF_R32G32_Float, 24),
		};

		InputLayoutPtr meshInputLayout = mGraphicsDriver->CreateInputLayout(meshElements, 3, mMeshShaders.at(EMS_Basic)->GetCompiledVS());
		mInputLayoutCache->RegisterLayout("positionnormaltexcoord", meshInputLayout);
	}

	// Load phong shader and create appropriate input layout
	{
		mMeshShaders.emplace(EMS_Phong, mGame.GetAssetCache().Load<Shader>("Shaders/Phong.hlsl"));

		InputLayoutElement phongElements[] = {
			InputLayoutElement("POSITION", 0, EGF_R32G32B32_Float, 0),
			InputLayoutElement("NORMAL", 0, EGF_R32G32B32_Float, 12),
			InputLayoutElement("TEXCOORD", 0, EGF_R32G32_Float, 24),
		};

		InputLayoutPtr phongInputLayout = mGraphicsDriver->CreateInputLayout(phongElements, 3, mMeshShaders.at(EMS_Phong)->GetCompiledVS());
		mInputLayoutCache->RegisterLayout("positionnormaltexcoord", phongInputLayout);
	}

	// Load skinned shader and create appropriate input layout
	{
		mMeshShaders.emplace(EMS_Skinned, mGame.GetAssetCache().Load<Shader>("Shaders/Skinned.hlsl"));

		InputLayoutElement skinnedElements[] = {
			InputLayoutElement("POSITION", 0, EGF_R32G32B32_Float, 0),
			InputLayoutElement("NORMAL", 0, EGF_R32G32B32_Float, 12),
			InputLayoutElement("BONES", 0, EGF_R8G8B8A8_UInt, 24),
			InputLayoutElement("WEIGHTS", 0, EGF_R8G8B8A8_UNorm, 28),
			InputLayoutElement("TEXCOORD", 0, EGF_R32G32_Float, 32),
		};

		InputLayoutPtr skinnedInputLayout = mGraphicsDriver->CreateInputLayout(skinnedElements, 5, mMeshShaders.at(EMS_Skinned)->GetCompiledVS());
		mInputLayoutCache->RegisterLayout("positionnormalbonesweightstexcoord", skinnedInputLayout);
		mMeshShaders.at(EMS_Skinned)->CreateMatrixPaletteBuffer();
	}

	// Load normal shader and create appropriate input layout
	{
		mMeshShaders.emplace(EMS_Normal, mGame.GetAssetCache().Load<Shader>("Shaders/Normal.hlsl"));

		InputLayoutElement normalElements[] = {
			InputLayoutElement("POSITION", 0, EGF_R32G32B32_Float, 0),
			InputLayoutElement("NORMAL", 0, EGF_R32G32B32_Float, 12),
			InputLayoutElement("NORMAL", 1, EGF_R32G32B32_Float, 24),
			InputLayoutElement("TEXCOORD", 0, EGF_R32G32_Float, 36),
		};

		InputLayoutPtr normalInputLayout = mGraphicsDriver->CreateInputLayout(normalElements, 4, mMeshShaders.at(EMS_Normal)->GetCompiledVS());
		mInputLayoutCache->RegisterLayout("positionnormaltangenttexcoord", normalInputLayout);
	}

	return true;
}

bool Renderer::InitSpriteVerts()
{
	// Create the vertex array for sprites
	float verts[] =
	{
		-0.5f, 0.5f, 0.0f, 0.0f, 0.0f,  // top left
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f,   // top right
		0.5f, -0.5f, 0.0f, 1.0f, 1.0f,  // bottom right
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom left
	};

	uint16_t indices[] =
	{
		0, 1, 2, // <top left, top right, bottom right>
		2, 3, 0, // <bottom right, bottom left, top left>
	};

	mSpriteVerts = VertexArray::Create(GetGraphicsDriver(), GetInputLayoutCache(),
		verts, 4, 20, "positiontexcoord", indices, 6);

	return true;
}
