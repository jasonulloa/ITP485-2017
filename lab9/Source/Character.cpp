#include "ITPEnginePCH.h"
#include "Character.h"

IMPL_ACTOR(Character, Actor);

Character::Character(Game& game)
	:Actor(game)
{
	mCharMove = CharacterMoveComponent::Create(*this, Component::PreTick);

	mMesh = SkeletalMeshComponent::Create(*this);

	// Setup animations for character move component
	mCharMove->SetOwnerMesh(mMesh);
}

void Character::BeginPlay()
{
	Super::BeginPlay();
}

void Character::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
}

void Character::EndPlay()
{
}

void Character::BeginTouch(Actor& other)
{
}

void Character::OnRespawn()
{
	SetPosition(mRespawnPosition);
}

void Character::SetProperties(const rapidjson::Value& properties)
{
	Super::SetProperties(properties);

	GetVectorFromJSON(properties, "respawnPosition", mRespawnPosition);
}