#pragma once
#include "Math.h"

const size_t MAX_SKELETON_BONES = 96;

struct MatrixPalette
{
	// TODO Lab 4e
	Matrix4 skinMatrix[MAX_SKELETON_BONES];
};
