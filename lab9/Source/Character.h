// Character.h
// Character actor

#pragma once
#include "Actor.h"
#include "Texture.h"
#include "CharacterMoveComponent.h"
#include "SkeletalMeshComponent.h"
#include "Skeleton.h"

class Character : public Actor
{
	DECL_ACTOR(Character, Actor);
public:
	Character(Game& game);

	void BeginPlay() override;
	void Tick(float deltaTime) override;
	void EndPlay() override;

	void BeginTouch(Actor& other) override;

	void OnRespawn();

	void SetProperties(const rapidjson::Value& properties) override;
private:
	Vector3 mRespawnPosition;

	// Need pointer to my sprite component to change texture
	CharacterMoveComponentPtr mCharMove;
	SkeletalMeshComponentPtr mMesh;

	SkeletonPtr mSkeleton;
};

DECL_PTR(Character);