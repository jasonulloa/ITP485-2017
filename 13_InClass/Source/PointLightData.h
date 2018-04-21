#pragma once
#include "Math.h"

const size_t MAX_POINT_LIGHTS = 8;

struct PointLightData
{
	PointLightData();

	// TODO Lab 3d
	Vector3 diffuseColor;
	float pad0;
	Vector3 specularColor;
	float pad1;
	Vector3 position;
	float specularPower;
	float innerRadius;
	float outerRadius;
	uint32_t isEnabled;
	float pad2;
};
