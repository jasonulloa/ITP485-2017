#include "ITPEnginePCH.h"
#include "Profiler.h"

Texture::Texture(class Game& game)
	:Asset(game)
	,mWidth(-1)
	,mHeight(-1)
	,mGraphicsDriver(game.GetRenderer().GetGraphicsDriver())
{

}

Texture::~Texture()
{

}

void Texture::SetActive(int slot)
{
	// TODO Lab 2m SetPSTexture
	mGraphicsDriver.SetPSTexture(mTexture, slot);
}

bool Texture::Load(const char* fileName, class AssetCache* cache)
{
	PROFILE_SCOPE(Texture_Load);
	// TODO Lab 2i
	mTexture = mGraphicsDriver.CreateTextureFromFile(fileName, mWidth, mHeight);
	return nullptr != mTexture.get();
}

std::shared_ptr<Texture> Texture::CreateFromSurface(class Game& game, struct SDL_Surface* surface)
{
	TexturePtr tex = std::make_shared<Texture>(game);
	tex->mWidth = surface->w;
	tex->mHeight = surface->h;

	tex->mTexture = game.GetRenderer().GetGraphicsDriver().CreateTextureFromMemory(
		surface->pixels, tex->mWidth, tex->mHeight, EXT_BGRA);
	return tex;
}
