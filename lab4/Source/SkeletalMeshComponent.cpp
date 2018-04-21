#include "ITPEnginePCH.h"

IMPL_COMPONENT(SkeletalMeshComponent, MeshComponent, 32);

SkeletalMeshComponent::SkeletalMeshComponent(Actor& owner)
	:MeshComponent(owner)
{

}

void SkeletalMeshComponent::Draw(class Renderer& render)
{
	if (mMesh) {
		render.DrawSkeletalMesh(mMesh->GetVertexArray(), mMesh->GetTexture(mTextureIndex), mOwner.GetWorldTransform(), mPalette);
	}
}

void SkeletalMeshComponent::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (mAnimation != nullptr) {
		mAnimationTime += deltaTime * mAnimationPlayRate;
	}

	float length = mAnimation->GetLength();

	if (mAnimationTime > length) {
		mAnimationTime -= length;
	}

	ComputeMatrixPalette();
}

float SkeletalMeshComponent::PlayAnimation(AnimationPtr anim, float playRate, float blendTime)
{
	DbgAssert(mSkeleton != nullptr, "Can't play an animation without a skeleton!");
	DbgAssert(mSkeleton->GetNumBones() == anim->GetNumBones(), 
		"Skeleton and animation have a different number of bones!");

	mAnimation = anim;
	mAnimationPlayRate = playRate;
	mAnimationTime = blendTime;
	ComputeMatrixPalette();

	return mAnimation->GetLength();
}

void SkeletalMeshComponent::SetProperties(const rapidjson::Value& properties)
{
	Super::SetProperties(properties);

	std::string skeleton;
	if (GetStringFromJSON(properties, "skeleton", skeleton))
	{
		mSkeleton = mOwner.GetGame().GetAssetCache().Load<Skeleton>(skeleton);
	}
}

void SkeletalMeshComponent::ComputeMatrixPalette()
{
	std::vector<Matrix4> invPoses, globalPoses;
	invPoses = mSkeleton->GetGlobalInvBindPoses();
	mAnimation->GetGlobalPoseAtTime(globalPoses, mSkeleton, mAnimationTime);
	for (int i = 0; i < invPoses.size(); i++) {
		mPalette.mPalette[i] = invPoses[i] * globalPoses[i];
	}
}
