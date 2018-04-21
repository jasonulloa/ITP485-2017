#include "ITPEnginePCH.h"

#define ARRAY_SIZE(array)	sizeof(array)/sizeof(array[0])

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

	return true;
}

void Renderer::RenderFrame()
{
	Clear();
	DrawComponents();
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
	// TODO Lab 3e
	mPointLights.emplace(light);
}

void Renderer::RemovePointLight(PointLightComponentPtr light)
{
	// TODO Lab 3e
	mPointLights.erase(light);
}

void Renderer::UpdatePointLights()
{
	// TODO Lab 3e
	PointLightData data[MAX_POINT_LIGHTS];
	int lightIndex = 0;
	for (const auto& pointLight : mPointLights)
	{
		data[lightIndex] = pointLight->mData;
		++lightIndex;
	}
	for (; lightIndex < MAX_POINT_LIGHTS; ++lightIndex)
		data[lightIndex].isEnabled = false;

	for (auto& shader : mMeshShaders)
	{
		Shader::LightingConstants &lightConst = shader.second->GetLightingConstants();
		for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
			lightConst.pointLight[i] = data[i];
	}
}

void Renderer::DrawSprite(TexturePtr texture, const Matrix4& worldTransform)
{
	// TODO Lab 2e set the sprite shader as active
	mSpriteShader->SetActive();

	// TODO Lab 2h Set the world transform
	mSpriteShader->GetPerObjectConstants().modelToWorld = worldTransform;
	mSpriteShader->UploadPerObjectConstants();

	// TODO Lab 2m Bind the texture
	mSpriteShader->BindTexture(texture, 0);

	// TODO Lab 2e Draw the vertex array
	DrawVertexArray(mSpriteVerts);
}

void Renderer::DrawMesh(VertexArrayPtr vertArray, TexturePtr texture, const Matrix4& worldTransform, EMeshShader type /*= EMS_Basic*/)
{
	// TODO Lab 2q
	// TODO Lab 3b replace hard-coded EMS_Basic with type
	Shader* pShader = mMeshShaders[type].get();
	pShader->SetActive();
	pShader->GetPerObjectConstants().modelToWorld = worldTransform;
	pShader->UploadPerObjectConstants();
	// TODO Lab 3c Upload Lighting Constants
	pShader->UploadLightingConstants();
	pShader->BindTexture(texture, 0);
	DrawVertexArray(vertArray);
}

void Renderer::DrawSkeletalMesh(VertexArrayPtr vertArray, TexturePtr texture, const Matrix4& worldTransform, const struct MatrixPalette& palette)
{
	// TODO Lab 4d
	// TODO Lab 4j
}

void Renderer::DrawVertexArray(VertexArrayPtr vertArray)
{
	vertArray->SetActive();
	mGraphicsDriver->DrawIndexed(vertArray->GetIndexCount(), 0, 0);
}

void Renderer::UpdateViewMatrix(const Matrix4& view)
{
	// TODO Lab 2p
	mView = view;
	Matrix4 viewProj = mView * mProj;

	// TODO Lab 3j
	Matrix4 invView = view;
	invView.Invert();
	Vector3 cameraPos = invView.GetTranslation();

	for (auto& shader : mMeshShaders)
	{
		shader.second->GetPerCameraConstants().viewProj = viewProj;
		shader.second->GetPerCameraConstants().cameraPosition = cameraPos;
	}
}

void Renderer::SetAmbientLight(const Vector3& color)
{
	// TODO Lab 3c
	for (auto& shader : mMeshShaders)
		shader.second->GetLightingConstants().ambient = color;
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
	// TODO Lab 2a
	mGraphicsDriver->ClearBackBuffer(Vector3(0.0f, 0.0f, 0.0f), 1.0f);
	// TODO Lab 2r clear the depth buffer
	mGraphicsDriver->ClearDepthStencil(mDepthBuffer, 1.0f);
}

void Renderer::DrawComponents()
{
	// TODO Lab 2r
	// Enable depth buffering
	mGraphicsDriver->SetDepthStencilState(mMeshDepthState);
	// TODO Lab 2s
	// Disable blending
	mGraphicsDriver->SetBlendState(mMeshBlendState);

	// TODO Lab 2p
	// Upload per camera constants
	for (auto& shader : mMeshShaders)
		shader.second->UploadPerCameraConstants();
	
	// TODO Lab 3e
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

	// TODO Lab 2r
	// Disable depth buffering
	mGraphicsDriver->SetDepthStencilState(mSpriteDepthState);
	// TODO Lab 2s
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
	// TODO Lab 2a
	mGraphicsDriver->Present();
}

bool Renderer::InitFrameBuffer()
{
	// TODO Lab 2l
	mRasterState = mGraphicsDriver->CreateRasterizerState(EFM_Solid);
	mGraphicsDriver->SetRasterizerState(mRasterState);
	// TODO Lab 2r set up depth buffers
	mDepthBuffer = mGraphicsDriver->CreateDepthStencil(mWidth, mHeight);
	mGraphicsDriver->SetDepthStencil(mDepthBuffer);
	mMeshDepthState = mGraphicsDriver->CreateDepthStencilState(true, ECF_LessEqual);
	mSpriteDepthState = mGraphicsDriver->CreateDepthStencilState(false, ECF_Always);
	// TODO Lab 2s set up the blend states
	mMeshBlendState = mGraphicsDriver->CreateBlendState(false);
	mSpriteBlendState = mGraphicsDriver->CreateBlendState(true);
	return true;
}

bool Renderer::InitShaders()
{
	// TODO Lab 2c
	// Load sprite shader
	{
		const InputLayoutElement s_spriteElem[] =
		{
			{ "POSITION", 0, EGF_R32G32B32_Float, 0 },
			{ "TEXCOORD", 0, EGF_R32G32_Float, sizeof(Vector3) },
		};
		mSpriteShader = mGame.GetAssetCache().Load<Shader>("Shaders/Sprite.hlsl");
		if (nullptr == mSpriteShader)
			return false;
		InputLayoutPtr input = mGraphicsDriver->CreateInputLayout(s_spriteElem, ARRAY_SIZE(s_spriteElem), mSpriteShader->GetCompiledVS());
		mInputLayoutCache->RegisterLayout("positiontexcoord", input);
	}

	// TODO Lab 2g
	// Set up the orthographic view matrix
	mSpriteShader->GetPerCameraConstants().viewProj = Matrix4::CreateOrtho((float)mWidth, (float)mHeight, 1000.0f, -1000.0f);
	mSpriteShader->UploadPerCameraConstants();

	// TODO Lab 2o
	// Set up the projection matrix
	mProj = Matrix4::CreatePerspectiveFOV(Math::ToRadians(70.0f),
		(float)mWidth, (float)mHeight,
		25.0f, 10000.0f);

	// Load and set up the BasicMesh shader and create appropriate input layout
	{
		// TODO Lab 2o
		const InputLayoutElement s_basicElem[] =
		{
			{ "POSITION", 0, EGF_R32G32B32_Float, 0 },
			{ "NORMAL", 0, EGF_R32G32B32_Float, sizeof(Vector3) },
			{ "TEXCOORD", 0, EGF_R32G32_Float, 2 * sizeof(Vector3) },
		};
		ShaderPtr shader = mGame.GetAssetCache().Load<Shader>("Shaders/BasicMesh.hlsl");
		if (nullptr == shader)
			return false;
		mMeshShaders[EMS_Basic] = shader;
		InputLayoutPtr input = mGraphicsDriver->CreateInputLayout(s_basicElem, ARRAY_SIZE(s_basicElem), shader->GetCompiledVS());
		mInputLayoutCache->RegisterLayout("positionnormaltexcoord", input);
	}

	// Load phong shader and create appropriate input layout
	{
		// TODO Lab 3b
		ShaderPtr shader = mGame.GetAssetCache().Load<Shader>("Shaders/Phong.hlsl");
		if (nullptr == shader)
			return false;
		mMeshShaders[EMS_Phong] = shader;
	}

	// Load skinned shader and create appropriate input layout
	{
		// TODO Lab 4c
		// TODO Lab 4j
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
