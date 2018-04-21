#include "ITPEnginePCH.h"
#include <SDL/SDL_mixer.h>
#include "Player.h"
#include "Profiler.h"

Game::Game()
	:mRenderer(*this)
	,mAssetCache(*this, "Assets/")
	,mShouldQuit(false)
{

}

Game::~Game()
{
	mAssetCache.Clear();
	mWorld.RemoveAllActors();
	Mix_CloseAudio();
	TTF_Quit();
	SDL_Quit();
}

bool Game::Init()
{
	PROFILE_SCOPE(Game_Init);

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Failed to initialize SDL.");
		return false;
	}

	// Initialize Renderer
	if (!mRenderer.Init(1024, 768))
	{
		SDL_Log("Failed to initialize renderer.");
		return false;
	}

	// Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
	{
		SDL_Log("Failed to initialize SDL_mixer.");
		return false;
	}

	// Initialize SDL_ttf
	if (TTF_Init() != 0)
	{
		SDL_Log("Failed to initialize SDL_ttf");
		return false;
	}

	// Initialize RNG
	Random::Init();

	// Add input mappings
	AddInputMappings();

	// Start frame timer
	mTimer.Start();

	// Run any code at game start
	StartGame();

	return true;
}

void Game::RunLoop()
{
	while (!mShouldQuit)
	{
		ProcessInput();
		Tick();
		GenerateOutput();
	}
}

void Game::Quit()
{
	mShouldQuit = true;
}

void Game::StartGame()
{
#if 1	// TODO Lab 3k
	// TODO Lab 4a
	// TODO Lab 5a
	LevelLoader loader(*this);
	loader.Load("Assets/Levels/normalMap.itplevel");
#else
	{	// Setup camera
		auto cameraActor = Actor::Spawn(*this);
		auto cameraC = CameraComponent::Create(*cameraActor);
		cameraC->SetHorizontalDist(300.0f, 300.0f);
		cameraC->SetVerticalDist(0.0f, 0.0f);
		cameraC->SetTargetOffset(0.0f);
		cameraC->SetSpringConstant(100.0f);
		cameraC->SnapToIdealPosition();
	}

#if 0 // TODO Lab 2n change this to a #if 0
	{	// Spawn a sprite
		auto sprite = Actor::Spawn(*this);
		auto spriteC = SpriteComponent::Create(*sprite);
		spriteC->SetTexture(mAssetCache.Load<Texture>("Textures/Cube.png"));
		// TODO Lab 2k set the position, rotation, and scale of the sprite
	}
#else
	{	// Spawn a sprite
		auto sprite = Actor::Spawn(*this);
		sprite->SetScale(0.25f);
		sprite->SetPosition(Vector3(400.0f, -100.0f, 0.0f));
		sprite->SetRotation(Quaternion(Vector3::UnitZ, Math::ToRadians(90.0f)));
		auto spriteC = SpriteComponent::Create(*sprite);
		spriteC->SetTexture(mAssetCache.Load<Texture>("Textures/Cube.png"));
	}
	{
		auto ship = Actor::Spawn(*this);
		ship->SetRotation(Quaternion(Vector3::UnitY, Math::ToRadians(-45.0f)));
		auto shipMC = MeshComponent::Create(*ship);
		shipMC->SetMesh(mAssetCache.Load<Mesh>("Meshes/PlayerShip.itpmesh2"));
	}
	{
		auto arrow = Actor::Spawn(*this);
		arrow->SetPosition(Vector3(0.0f, -200.0f, 100.0f));
		arrow->SetScale(5.0f);
		auto arrowMC = MeshComponent::Create(*arrow);
		arrowMC->SetMesh(mAssetCache.Load<Mesh>("Meshes/Arrow.itpmesh2"));
	}
#endif

	// TODO Lab 3c
	// Set ambient light
	GetRenderer().SetAmbientLight(Vector3(0.1f, 0.1f, 0.1f));

	// TODO Lab 3f
	{   // Set up Point Light
		auto light = Actor::Spawn(*this);
		light->SetPosition(Vector3(0.0f, 0.0f, 50.0f));
		auto lightC = PointLightComponent::Create(*light);
		lightC->mData.diffuseColor = Vector3(0.0f, 1.0f, 0.0f);
		lightC->mData.specularColor = Vector3(0.5f, 1.0f, 0.5f);
		lightC->mData.specularPower = 10.0f;
		lightC->mData.innerRadius = 10.0f;
		lightC->mData.outerRadius = 100.0f;
	}
	// TODO Lab 3i
	{   // Second Point Light
		auto light = Actor::Spawn(*this);
		light->SetPosition(Vector3(-25.0f, 0.0f, 25.0f));
		auto lightC = PointLightComponent::Create(*light);
		lightC->mData.diffuseColor = Vector3(0.0f, 0.0f, 1.0f);
		lightC->mData.specularColor = Vector3(0.5f, 0.5f, 1.0f);
		lightC->mData.specularPower = 10.0f;
		lightC->mData.innerRadius = 10.0f;
		lightC->mData.outerRadius = 100.0f;
	}
#endif
}

void Game::ProcessInput()
{
	// Poll events from SDL
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mShouldQuit = true;
			break;
		case SDL_KEYDOWN:
			if (event.key.repeat == 0)
			{
				HandleKeyPressed(event.key.keysym.sym);
			}
			break;
		case SDL_KEYUP:
			HandleKeyReleased(event.key.keysym.sym);
			break;
		default:
			// Ignore other events
			break;
		}
	}
}

void Game::HandleKeyPressed(int key)
{
	mInput.HandleKeyPressed(key);
}

void Game::HandleKeyReleased(int key)
{
	mInput.HandleKeyReleased(key);
}

void Game::Tick()
{
	{
		PROFILE_SCOPE(Game_Tick);

		// Lock @ 60 FPS
		float deltaTime = mTimer.GetFrameTime(0.016666f);

		{
			PROFILE_SCOPE(Game_mGameTimers);
			mGameTimers.Tick(deltaTime);
		}

		{	// Update game world
			PROFILE_SCOPE(Game_mWorld);
			mWorld.Tick(deltaTime);
		}

		{	// Update physics world
			PROFILE_SCOPE(Game_mPhysWorld);
			mPhysWorld.Tick(deltaTime);
		}
	}
	Profiler::Get()->ResetAll();
}

void Game::GenerateOutput()
{
	mRenderer.RenderFrame();
}

void Game::AddInputMappings()
{
	// Parse input mappings
	mInput.ParseMappings("Config/Mappings.itpconfig");

	// Bind our quit function to "Quit" on release
	mInput.BindAction("Quit", IE_Released, this, &Game::Quit);
}
