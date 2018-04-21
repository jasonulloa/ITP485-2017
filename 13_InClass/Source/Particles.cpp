#include "ITPEnginePCH.h"
#include "Particles.h"
#include "Profiler.h"

#define STRUCT_OF_ARRAYS
#define SIMD

//SofA 0          1          0           1
//SIMD 0          0          1           1
//Avg  4.740401   3.184201   7.338352    4.642828	// TODO 13_InClass fill these in
//Max  77.059036  38.409706  146.702881  66.309593	// TODO 13_InClass fill these in

static const int MAX_PARTICLES = 1024 * 100;
struct ColorVec
{
	float r, g, b, a;
};
static const float s_speed = 1.0f;
static const ColorVec s_startColor = { 0.0f, 1.0f, 0.0f, 1.0f };
static const ColorVec s_endColor = { 1.0f, 0.0f, 0.0f, 0.0f };
static const float s_lifeTime = 1.0f;

#ifdef STRUCT_OF_ARRAYS
struct Particles
{
	int mID[MAX_PARTICLES];
#ifdef SIMD
	__m128 mPosition[MAX_PARTICLES];
	__m128 mVelocity[MAX_PARTICLES];
	__m128 mColor[MAX_PARTICLES];
#else
	Vector3 mPosition[MAX_PARTICLES];
	Vector3 mVelocity[MAX_PARTICLES];
	ColorVec mColor[MAX_PARTICLES];
#endif
};

Particles gAllParticles;

void Particles_Init()
{
	for (int i = 0; i < MAX_PARTICLES; ++i)
	{
		float ang = Random::GetFloatRange(-Math::Pi, Math::Pi);
		gAllParticles.mID[i] = i;
#ifdef SIMD
		gAllParticles.mPosition[i] = _mm_setzero_ps();
		gAllParticles.mVelocity[i] = _mm_set_ps(s_speed * cosf(ang), s_speed * sinf(ang), 0.0f, 0.0f);
		gAllParticles.mColor[i] = _mm_set_ps(s_startColor.r, s_startColor.g, s_startColor.b, s_startColor.a);
#else
		gAllParticles.mPosition[i] = Vector3::Zero;
		gAllParticles.mVelocity[i] = s_speed * Vector3(cosf(ang), sinf(ang), 0.0f);
		gAllParticles.mColor[i] = s_startColor;
#endif
	}
}

void Particles_Tick(float deltaTime)
{
	PROFILE_SCOPE(Particles_Tick);

#ifdef SIMD
	__m128 colorDelta = _mm_set_ps(
		deltaTime * (s_endColor.r - s_startColor.r) / s_lifeTime,
		deltaTime * (s_endColor.g - s_startColor.g) / s_lifeTime,
		deltaTime * (s_endColor.b - s_startColor.b) / s_lifeTime,
		deltaTime * (s_endColor.a - s_startColor.a) / s_lifeTime
	);
	__m128 deltaV = _mm_set_ps(deltaTime, deltaTime, deltaTime, deltaTime);
	for (int i = 0; i < MAX_PARTICLES; ++i)
	{
		gAllParticles.mPosition[i] = _mm_add_ps(gAllParticles.mPosition[i], _mm_mul_ps(gAllParticles.mVelocity[i], deltaV));
		gAllParticles.mColor[i] = _mm_add_ps(gAllParticles.mColor[i], colorDelta);
	}
#else
	ColorVec colorDelta = s_endColor;
	colorDelta.r = deltaTime * (s_endColor.r - s_startColor.r) / s_lifeTime;
	colorDelta.g = deltaTime * (s_endColor.g - s_startColor.g) / s_lifeTime;
	colorDelta.b = deltaTime * (s_endColor.b - s_startColor.b) / s_lifeTime;
	colorDelta.a = deltaTime * (s_endColor.a - s_startColor.a) / s_lifeTime;
	for (int i = 0; i < MAX_PARTICLES; ++i)
	{
		gAllParticles.mPosition[i] += gAllParticles.mVelocity[i] * deltaTime;
		gAllParticles.mColor[i].r += colorDelta.r;
		gAllParticles.mColor[i].g += colorDelta.g;
		gAllParticles.mColor[i].b += colorDelta.b;
		gAllParticles.mColor[i].a += colorDelta.a;
	}
#endif
}

#else

struct Particle
{
	int mID;
#ifdef SIMD
	__m128 mPosition;
	__m128 mVelocity;
	__m128 mColor;
#else
	Vector3 mPosition;
	Vector3 mVelocity;
	ColorVec mColor;
#endif
};

Particle gParticles[MAX_PARTICLES];

void Particles_Init()
{
	for (int i = 0; i < MAX_PARTICLES; ++i)
	{
		float ang = Random::GetFloatRange(-Math::Pi, Math::Pi);
		gParticles[i].mID = i;
#ifdef SIMD
		gParticles[i].mPosition = _mm_setzero_ps();
		gParticles[i].mVelocity = _mm_set_ps(s_speed * cosf(ang), s_speed * sinf(ang), 0.0f, 0.0f);
		gParticles[i].mColor = _mm_set_ps(s_startColor.r, s_startColor.g, s_startColor.b, s_startColor.a);
#else
		gParticles[i].mPosition = Vector3::Zero;
		gParticles[i].mVelocity = s_speed * Vector3(cosf(ang), sinf(ang), 0.0f);
		gParticles[i].mColor = s_startColor;
#endif
	}
}

void Particles_Tick(float deltaTime)
{
	PROFILE_SCOPE(Particles_Tick);

#ifdef SIMD
	__m128 colorDelta = _mm_set_ps(
		deltaTime * (s_endColor.r - s_startColor.r) / s_lifeTime,
		deltaTime * (s_endColor.g - s_startColor.g) / s_lifeTime,
		deltaTime * (s_endColor.b - s_startColor.b) / s_lifeTime,
		deltaTime * (s_endColor.a - s_startColor.a) / s_lifeTime
	);
	__m128 deltaV = _mm_set_ps(deltaTime, deltaTime, deltaTime, deltaTime);
	for (int i = 0; i < MAX_PARTICLES; ++i)
	{
		gParticles[i].mPosition = _mm_add_ps(gParticles[i].mPosition, _mm_mul_ps(gParticles[i].mVelocity, deltaV));
		gParticles[i].mColor = _mm_add_ps(gParticles[i].mColor, colorDelta);
	}
#else
	ColorVec colorDelta = s_endColor;
	colorDelta.r = deltaTime * (s_endColor.r - s_startColor.r) / s_lifeTime;
	colorDelta.g = deltaTime * (s_endColor.g - s_startColor.g) / s_lifeTime;
	colorDelta.b = deltaTime * (s_endColor.b - s_startColor.b) / s_lifeTime;
	colorDelta.a = deltaTime * (s_endColor.a - s_startColor.a) / s_lifeTime;
	for (int i = 0; i < MAX_PARTICLES; ++i)
	{
		gParticles[i].mPosition += gParticles[i].mVelocity * deltaTime;
		gParticles[i].mColor.r += colorDelta.r;
		gParticles[i].mColor.g += colorDelta.g;
		gParticles[i].mColor.b += colorDelta.b;
		gParticles[i].mColor.a += colorDelta.a;
	}
#endif
}

#endif

void Particles_Draw()
{
	// we don't bother drawing it because we only care about the update speed right now.
}

