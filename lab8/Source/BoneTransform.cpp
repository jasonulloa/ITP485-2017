#include "ITPEnginePCH.h"

Matrix4 BoneTransform::ToMatrix() const
{
	Matrix4 boneMatrix, boneRotMatrix, boneTransMatrix;
	boneRotMatrix = boneRotMatrix.CreateFromQuaternion(mRotation);
	boneTransMatrix = boneTransMatrix.CreateTranslation(mTranslation);
	boneMatrix = boneRotMatrix * boneTransMatrix;
	return boneMatrix;
}

BoneTransform Interpolate(const BoneTransform& a, const BoneTransform& b, float f)
{
	BoneTransform boned;
	Quaternion slerped;
	Vector3 lerped;
	slerped = Slerp(a.mRotation, b.mRotation, f);
	lerped = Lerp(a.mTranslation, b.mTranslation, f);
	boned.mRotation = slerped;
	boned.mTranslation = lerped;
	return boned;
}
