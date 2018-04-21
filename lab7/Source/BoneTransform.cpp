#include "ITPEnginePCH.h"
#include "Profiler.h"

Matrix4 BoneTransform::ToMatrix() const
{
	PROFILE_SCOPE(BoneTransform_ToMatrix);
	// TODO Lab 4d
	return Matrix4::CreateFromQuaternion(mRotation) * Matrix4::CreateTranslation(mTranslation);
}

BoneTransform Interpolate(const BoneTransform& a, const BoneTransform& b, float f)
{
	PROFILE_SCOPE(BoneTransform_Interpolate);
	// TODO Lab 4d
	BoneTransform result;
	result.mRotation = Slerp(a.mRotation, b.mRotation, f);
	result.mTranslation = Lerp(a.mTranslation, b.mTranslation, f);
	return result;
}
