#pragma once
#include "Math.h"
#include <xmmintrin.h>
#include <smmintrin.h>

// SHUFFLER is like shuffle, but has easier to understand indices
#define _MM_SHUFFLER( xi, yi, zi, wi ) _MM_SHUFFLE( wi, zi, yi, xi )

class alignas(16) SimdVector3
{
	// Underlying vector
	__m128 mVec;
public:
	// Empty default constructor
	SimdVector3() { }

	// Constructor from __m128
	explicit SimdVector3(__m128 vec)
	{
		mVec = vec;
	}

	// Constructor if converting from Vector3
	explicit SimdVector3(const Vector3& vec)
	{
		FromVector3(vec);
	}

	// Load from a Vector3 into this SimdVector3
	void FromVector3(const Vector3& vec)
	{
		// We can't assume this is aligned
		mVec = _mm_setr_ps(vec.x, vec.y, vec.z, 0.0f);
	}

	// Convert this SimdVector3 to a Vector3
	Vector3 ToVector3() const
	{
		return Vector3(mVec);
	}

	// this = this + other
	void Add(const SimdVector3& other)
	{
		// TODO Lab 1a
	}

	// this = this - other
	void Sub(const SimdVector3& other)
	{
		// TODO Lab 1a
	}

	// this = this * other (componentwise)
	void Mul(const SimdVector3& other)
	{
		// TODO Lab 1a
	}

	// this = this * scalar
	void Mul(float scalar)
	{
		// TODO Lab 1a
	}

	// Normalize this vector
	void Normalize()
	{
		// TODO Lab 1a
	}

	// (this dot other), storing the dot product
	// in EVERY COMPONENT of returned SimdVector3
	SimdVector3 Dot(const SimdVector3& other) const
	{
		// TODO Lab 1a
		return SimdVector3();
	}

	// Length Squared of this, storing the result in
	// EVERY COMPONENT of returned SimdVector3
	SimdVector3 LengthSq() const
	{
		// TODO Lab 1a
		return SimdVector3();
	}

	// Length of this, storing the result in
	// EVERY COMPONENT of returned SimdVector3
	SimdVector3 Length() const
	{
		// TODO Lab 1a
		return SimdVector3();
	}

	// result = this (cross) other
	SimdVector3 Cross(const SimdVector3& other) const
	{
		// TODO Lab 1a
		return SimdVector3();
	}

	// result = this * (1.0f - f) + other * f
	SimdVector3 Lerp(const SimdVector3& other, float f) const
	{
		// TODO Lab 1a
		return SimdVector3();
	}

	friend SimdVector3 Transform(const SimdVector3& vec, const class SimdMatrix4& mat, float w);
};

class alignas(16) SimdMatrix4
{
	// Four vectors, one for each row
	__m128 mRows[4];
public:
	// Empty default constructor
	SimdMatrix4() { }

	// Constructor from array of four __m128s
	explicit SimdMatrix4(__m128 rows[4])
	{
		memcpy(mRows, rows, sizeof(__m128) * 4);
	}

	// Constructor if converting from Matrix4
	explicit SimdMatrix4(const Matrix4& mat)
	{
		FromMatrix4(mat);
	}

	// Load from a Matrix4 into this SimdMatrix4
	void FromMatrix4(const Matrix4& mat)
	{
		// We can't assume that mat is aligned, so
		// we can't use mm_set_ps
		memcpy(mRows, mat.mat, sizeof(float) * 16);
	}

	// Convert this SimdMatrix4 to a Matrix4
	Matrix4 ToMatrix4()
	{
		return Matrix4(mRows);
	}

	// this = this * other
	void Mul(const SimdMatrix4& other)
	{
		// TODO Lab 1c
	}

	// Transpose this matrix
	void Transpose()
	{
		_MM_TRANSPOSE4_PS(mRows[0], mRows[1], mRows[2], mRows[3]);
	}

	// Loads a Scale matrix into this
	void LoadScale(float scale)
	{
		// scale 0 0 0
		mRows[0] = _mm_set_ss(scale);
		mRows[0] = _mm_shuffle_ps(mRows[0], mRows[0], _MM_SHUFFLE(1, 1, 1, 0));

		// 0 scale 0 0
		mRows[1] = _mm_set_ss(scale);
		mRows[1] = _mm_shuffle_ps(mRows[1], mRows[1], _MM_SHUFFLE(1, 1, 0, 1));

		// 0 0 scale 0
		mRows[2] = _mm_set_ss(scale);
		mRows[2] = _mm_shuffle_ps(mRows[2], mRows[2], _MM_SHUFFLE(1, 0, 1, 1));

		// 0 0 0 1
		mRows[3] = _mm_set_ss(1.0f);
		mRows[3] = _mm_shuffle_ps(mRows[3], mRows[3], _MM_SHUFFLE(0, 1, 1, 1));
	}

	// Loads a rotation about the X axis into this
	void LoadRotationX(float angle)
	{
		// 1 0 0 0
		mRows[0] = _mm_set_ss(1.0f);
		mRows[0] = _mm_shuffle_ps(mRows[0], mRows[0], _MM_SHUFFLE(1, 1, 1, 0));

		float cosTheta = Math::Cos(angle);
		float sinTheta = Math::Sin(angle);

		// 0 cos sin 0
		mRows[1] = _mm_setr_ps(0.0f, cosTheta, sinTheta, 0.0f);

		// 0 -sin cos 0
		mRows[2] = _mm_setr_ps(0.0f, -sinTheta, cosTheta, 0.0f);

		// 0 0 0 1
		mRows[3] = _mm_set_ss(1.0f);
		mRows[3] = _mm_shuffle_ps(mRows[3], mRows[3], _MM_SHUFFLE(0, 1, 1, 1));
	}

	// Loads a rotation about the Y axis into this
	void LoadRotationY(float angle)
	{
		float cosTheta = Math::Cos(angle);
		float sinTheta = Math::Sin(angle);

		// cos 0 -sin 0
		mRows[0] = _mm_setr_ps(cosTheta, 0.0f, -sinTheta, 0.0f);

		// 0 1 0 0
		mRows[1] = _mm_set_ss(1.0f);
		mRows[1] = _mm_shuffle_ps(mRows[1], mRows[1], _MM_SHUFFLER(1, 0, 1, 1));

		// sin 0 cos 0
		mRows[2] = _mm_setr_ps(sinTheta, 0.0f, cosTheta, 0.0f);

		// 0 0 0 1
		mRows[3] = _mm_set_ss(1.0f);
		mRows[3] = _mm_shuffle_ps(mRows[3], mRows[3], _MM_SHUFFLE(0, 1, 1, 1));
	}

	// Loads a rotation about the Z axis into this
	void LoadRotationZ(float angle)
	{
		float cosTheta = Math::Cos(angle);
		float sinTheta = Math::Sin(angle);

		// cos sin 0 0
		mRows[0] = _mm_setr_ps(cosTheta, sinTheta, 0.0f, 0.0f);

		// -sin cos 0 0
		mRows[1] = _mm_setr_ps(-sinTheta, cosTheta, 0.0f, 0.0f);

		// 0 0 1 0
		mRows[2] = _mm_set_ss(1.0f);
		mRows[2] = _mm_shuffle_ps(mRows[2], mRows[2], _MM_SHUFFLER(1, 1, 0, 1));

		// 0 0 0 1
		mRows[3] = _mm_set_ss(1.0f);
		mRows[3] = _mm_shuffle_ps(mRows[3], mRows[3], _MM_SHUFFLE(0, 1, 1, 1));
	}

	// Loads a translation matrix into this
	void LoadTranslation(const Vector3& trans)
	{
		// 1 0 0 0
		mRows[0] = _mm_set_ss(1.0f);
		mRows[0] = _mm_shuffle_ps(mRows[0], mRows[0], _MM_SHUFFLER(0, 1, 1, 1));

		// 0 1 0 0
		mRows[1] = _mm_set_ss(1.0f);
		mRows[1] = _mm_shuffle_ps(mRows[1], mRows[1], _MM_SHUFFLER(1, 0, 1, 1));

		// 0 0 1 0
		mRows[2] = _mm_set_ss(1.0f);
		mRows[2] = _mm_shuffle_ps(mRows[2], mRows[2], _MM_SHUFFLER(1, 1, 0, 1));

		mRows[3] = _mm_setr_ps(trans.x, trans.y, trans.z, 1.0f);
	}

	// Loads a matrix from a quaternion into this
	void LoadFromQuaternion(const Quaternion& quat);

	// Inverts this matrix
	void Invert();

	friend SimdVector3 Transform(const SimdVector3& vec, const class SimdMatrix4& mat, float w);
};

inline SimdVector3 Transform(const SimdVector3& vec, const SimdMatrix4& mat, float w = 1.0f)
{
	// TODO Lab 1b
	return SimdVector3();
}
