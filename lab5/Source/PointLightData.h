#pragma once
#include "Math.h"

const size_t MAX_POINT_LIGHTS = 8;

struct PointLightData
{
	PointLightData();

	Vector3 diffuseColor;
	float unused1 = 0.0f;
	Vector3 specularColor;
	float unused2 = 0.0f;
	Vector3 position;
	float specularPower;
	float innerRadius;
	float outerRadius;
	uint32_t enabled;
	float unused3 = 0.0f;
};
