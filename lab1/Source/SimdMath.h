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
		mVec = _mm_add_ps(mVec, other.mVec);
	}

	// this = this - other
	void Sub(const SimdVector3& other)
	{
		mVec = _mm_sub_ps(mVec, other.mVec);
	}

	// this = this * other (componentwise)
	void Mul(const SimdVector3& other)
	{
		mVec = _mm_mul_ps(mVec, other.mVec);
	}

	// this = this * scalar
	void Mul(float scalar)
	{
		__m128 temp = _mm_set_ps1(scalar);
		mVec = _mm_mul_ps(mVec, temp);
	}

	// Normalize this vector
	void Normalize()
	{
		__m128 temp = _mm_dp_ps(mVec, mVec, 0x77);
		temp = _mm_rsqrt_ps(temp);
		mVec = _mm_mul_ps(mVec, temp);
	}

	// (this dot other), storing the dot product
	// in EVERY COMPONENT of returned SimdVector3
	SimdVector3 Dot(const SimdVector3& other) const
	{
		__m128 temp = _mm_dp_ps(mVec, other.mVec, 0x77);
		return SimdVector3(temp);
	}

	// Length Squared of this, storing the result in
	// EVERY COMPONENT of returned SimdVector3
	SimdVector3 LengthSq() const
	{
		__m128 temp = _mm_dp_ps(mVec, mVec, 0x77);
		return SimdVector3(temp);
	}

	// Length of this, storing the result in
	// EVERY COMPONENT of returned SimdVector3
	SimdVector3 Length() const
	{
		__m128 temp = _mm_dp_ps(mVec, mVec, 0x77);
		temp = _mm_sqrt_ps(temp);
		return SimdVector3(temp);
	}

	// result = this (cross) other
	SimdVector3 Cross(const SimdVector3& other) const
	{
		__m128 tempA = _mm_shuffle_ps(mVec, mVec, _MM_SHUFFLER(1, 2, 0, 3));
		__m128 tempB = _mm_shuffle_ps(other.mVec, other.mVec, _MM_SHUFFLER(2, 0, 1, 3));
		__m128 result = _mm_mul_ps(tempA, tempB);
		tempA = _mm_shuffle_ps(mVec, mVec, _MM_SHUFFLER(2, 0, 1, 3));
		tempB = _mm_shuffle_ps(other.mVec, other.mVec, _MM_SHUFFLER(1, 2, 0, 3));
		tempA = _mm_mul_ps(tempA, tempB);
		result = _mm_sub_ps(result, tempA);
		return SimdVector3(result);
	}

	// result = this * (1.0f - f) + other * f
	SimdVector3 Lerp(const SimdVector3& other, float f) const
	{
		__m128 temp = _mm_set_ps1(1.0f - f);
		__m128 result = _mm_mul_ps(mVec, temp);
		temp = _mm_set_ps1(f);
		result = _mm_add_ps(result, _mm_mul_ps(other.mVec, temp));
		return SimdVector3(result);
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
		__m128 row0b = other.mRows[0];
		__m128 row1b = other.mRows[1];
		__m128 row2b = other.mRows[2];
		__m128 row3b = other.mRows[3];
		_MM_TRANSPOSE4_PS(row0b, row1b, row2b, row3b);
		__m128 Cx = _mm_dp_ps(mRows[0], row0b, 0xF1);
		__m128 Cy = _mm_dp_ps(mRows[0], row1b, 0xF2);
		__m128 Cz = _mm_dp_ps(mRows[0], row2b, 0xF4);
		__m128 Cw = _mm_dp_ps(mRows[0], row3b, 0xF8);
		__m128 row0 = _mm_add_ps(Cx, Cy);
		row0 = _mm_add_ps(row0, Cz);
		row0 = _mm_add_ps(row0, Cw);
		Cx = _mm_dp_ps(mRows[1], row0b, 0xF1);
		Cy = _mm_dp_ps(mRows[1], row1b, 0xF2);
		Cz = _mm_dp_ps(mRows[1], row2b, 0xF4);
		Cw = _mm_dp_ps(mRows[1], row3b, 0xF8);
		__m128 row1 = _mm_add_ps(Cx, Cy);
		row1 = _mm_add_ps(row1, Cz);
		row1 = _mm_add_ps(row1, Cw);
		Cx = _mm_dp_ps(mRows[2], row0b, 0xF1);
		Cy = _mm_dp_ps(mRows[2], row1b, 0xF2);
		Cz = _mm_dp_ps(mRows[2], row2b, 0xF4);
		Cw = _mm_dp_ps(mRows[2], row3b, 0xF8);
		__m128 row2 = _mm_add_ps(Cx, Cy);
		row2 = _mm_add_ps(row2, Cz);
		row2 = _mm_add_ps(row2, Cw);
		Cx = _mm_dp_ps(mRows[3], row0b, 0xF1);
		Cy = _mm_dp_ps(mRows[3], row1b, 0xF2);
		Cz = _mm_dp_ps(mRows[3], row2b, 0xF4);
		Cw = _mm_dp_ps(mRows[3], row3b, 0xF8);
		__m128 row3 = _mm_add_ps(Cx, Cy);
		row3 = _mm_add_ps(row3, Cz);
		row3 = _mm_add_ps(row3, Cw);
		mRows[0] = row0;
		mRows[1] = row1;
		mRows[2] = row2;
		mRows[3] = row3;
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
	__m128 vect = vec.mVec;
	__m128 temp = _mm_set_ss(w);
	vect = _mm_insert_ps(vect, temp, 0x30);
	__m128 row0 = mat.mRows[0];
	__m128 row1 = mat.mRows[1];
	__m128 row2 = mat.mRows[2];
	__m128 row3 = mat.mRows[3];
	_MM_TRANSPOSE4_PS(row0, row1, row2, row3);
	__m128 xprime = _mm_dp_ps(vect, row0, 0xF1);
	__m128 yprime = _mm_dp_ps(vect, row1, 0xF2);
	__m128 zprime = _mm_dp_ps(vect, row2, 0xF4);
	__m128 wprime = _mm_dp_ps(vect, row3, 0xF8);
	vect = _mm_add_ps(xprime, yprime);
	vect = _mm_add_ps(vect, zprime);
	vect = _mm_add_ps(vect, wprime);
	return SimdVector3(vect);
}