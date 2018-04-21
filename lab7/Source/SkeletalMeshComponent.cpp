#include "ITPEnginePCH.h"

IMPL_COMPONENT(SkeletalMeshComponent, MeshComponent, 32);

SkeletalMeshComponent::SkeletalMeshComponent(Actor& owner)
	:MeshComponent(owner)
{

}

void SkeletalMeshComponent::Draw(class Renderer& render)
{
	// TODO Lab 4c
	if (mMesh)
	{
		render.DrawSkeletalMesh(mMesh->GetVertexArray(), mMesh->GetTexture(mTextureIndex),
			mOwner.GetWorldTransform(), mPalette);
	}
}

void SkeletalMeshComponent::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	// TODO Lab 4i
	if (nullptr != mAnimation)
	{
		mAnimationTime += deltaTime * mAnimationPlayRate;
		while (mAnimationTime > mAnimation->GetLength())
			mAnimationTime -= mAnimation->GetLength();
		ComputeMatrixPalette();
	}
}

float SkeletalMeshComponent::PlayAnimation(AnimationPtr anim, float playRate /*= 1.0f*/, float blendTime /*= 0.0f*/)
{
	DbgAssert(mSkeleton != nullptr, "Can't play an animation without a skeleton!");
	DbgAssert(mSkeleton->GetNumBones() == anim->GetNumBones(), 
		"Skeleton and animation have a different number of bones!");

	// TODO Lab 4f Add a call to GetGlobalPoseAtTime

	// TODO Lab 4g Implement properly
	mAnimation = anim;
	mAnimationPlayRate = playRate;
	mAnimationTime = 0.0f;
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
	// TODO Lab 4g
	std::vector<Matrix4> pose;
	mAnimation->GetGlobalPoseAtTime(pose, mSkeleton, mAnimationTime);
	const std::vector<Matrix4>& invBind = mSkeleton->GetGlobalInvBindPoses();
	for (size_t i = 0; i < pose.size(); ++i)
	{
		mPalette.skinMatrix[i] = invBind[i] * pose[i];
	}
}
