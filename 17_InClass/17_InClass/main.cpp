#include <stdio.h>
#include "Math.h"

#define ARRAY_LENGTH(array) sizeof(array)/sizeof(array[0])

struct LineSegment
{
	Vector3 p0, p1;
};

struct Triangle
{
	Vector3 a, b, c;
};

//----------------------------------------------------------------------------------------
// Return true if the LineSegment "line" intersects the Triangle "tri" and false if not.
// If they intersect, store the point of intersection in "pPoint".
//
// This is currently implemented as Ray vs Plane only - it is up to you to provide the rest.
//----------------------------------------------------------------------------------------
bool LineSegmentVsTriangle(const LineSegment &line, const Triangle &tri, Vector3 *pPoint)
{
	// Calculate the normal of the triangle
	Vector3 ab = tri.b - tri.a;
	Vector3 ac = tri.c - tri.a;
	Vector3 n = Cross(ab, ac);
	n.Normalize();

	// follow along with slides 25-27 of 16_HardwareAnd3DMath to find the point of intersection between the ray and the plane.
	Vector3 v = line.p1 - line.p0;
	float vDotN = Dot(v, n);
	if (fabsf(vDotN) < 0.001f)
		return false;	// the plane and ray are parallel... just return false
	float d = -Dot(tri.a, n);
	float t = -(Dot(line.p0, n) + d) / vDotN;

	float t2 = -(Dot(line.p1, n) + d) / vDotN;

	if (((t < 0) && (t2 < 0)) || ((t > 0) && (t2 > 0)))
		return false;

	// Calculate the point of intersection
	Vector3 p = line.p0 + t * v;

	// TODO 17_InClass - Does the point of intersection lie within the triangle?
	Vector3 ap = p - tri.a;
	Vector3 bp = p - tri.b;
	Vector3 cp = p - tri.c;
	Vector3 abXap = Cross(ab, ap);
	Vector3 bcXbp = Cross((tri.c - tri.b), bp);
	Vector3 caXcp = Cross((tri.a - tri.c), cp);

	if (!((Dot(abXap, n) > 0) && (Dot(bcXbp, n) > 0) && (Dot(caXcp, n) > 0))) {
		return false;
	}

	// if the user has provided for an output point, fill it in
	if (nullptr != pPoint)
		*pPoint = p;
	return true;
}

int main(int argc, const char* argv[])
{
	// A series of line segments and triangles to test the function with
	struct Test
	{
		LineSegment line;	// the line segment to try
		Triangle tri;		// the triangle to try
		bool intersect;		// true if the line segment is SUPPOSED to intersect the triangle
	};
	const Test test[] = 
	{
		{ { Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 0.0f, 0.0f) },		{ Vector3(5.0f, -5.0f, -5.0f), Vector3(5.0f, -5.0f, 5.0f), Vector3(5.0f, 5.0f, 0.0f) },		true },
		{ { Vector3(0.0f, 0.0f, 0.0f), Vector3(-10.0f, 0.0f, 0.0f) },		{ Vector3(5.0f, -5.0f, -5.0f), Vector3(5.0f, -5.0f, 5.0f), Vector3(5.0f, 5.0f, 0.0f) },		false },
		{ { Vector3(5.0f, 0.0f, 0.0f), Vector3(5.0f, 10.0f, 0.0f) },		{ Vector3(5.0f, -5.0f, -5.0f), Vector3(5.0f, -5.0f, 5.0f), Vector3(5.0f, 5.0f, 0.0f) },		false },
		{ { Vector3(0.0f, 0.0f, 0.0f), Vector3(5.0f, 5.0f, 5.0f) },			{ Vector3(1.0f, 2.0f, 3.0f), Vector3(2.0f, 3.0f, 1.0f), Vector3(3.0f, 1.0f, 2.0f) },		true },
		{ { Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 10.0f) },		{ Vector3(5.0f, -5.0f, 5.0f), Vector3(-5.0f, -5.0f, 5.0f), Vector3(0.0f, 5.0f, 5.0f) },		true },
		{ { Vector3(0.0f, -4.99f, 0.0f), Vector3(0.0f, -4.99f, 10.0f) },	{ Vector3(5.0f, -5.0f, 5.0f), Vector3(-5.0f, -5.0f, 5.0f), Vector3(0.0f, 5.0f, 5.0f) },		true },
		{ { Vector3(0.0f, -5.01f, 0.0f), Vector3(0.0f, -5.01f, 10.0f) },	{ Vector3(5.0f, -5.0f, 5.0f), Vector3(-5.0f, -5.0f, 5.0f), Vector3(0.0f, 5.0f, 5.0f) },		false },
		{ { Vector3(0.0f, 4.99f, 0.0f), Vector3(0.0f, 4.99f, 10.0f) },		{ Vector3(5.0f, -5.0f, 5.0f), Vector3(-5.0f, -5.0f, 5.0f), Vector3(0.0f, 5.0f, 5.0f) },		true },
		{ { Vector3(0.0f, 5.01f, 0.0f), Vector3(0.0f, 5.01f, 10.0f) },		{ Vector3(5.0f, -5.0f, 5.0f), Vector3(-5.0f, -5.0f, 5.0f), Vector3(0.0f, 5.0f, 5.0f) },		false },
	};

	bool allOk = true;

	for (int i = 0; i < ARRAY_LENGTH(test); ++i)
	{
		printf("Test %d\n", i);
		Vector3 point;
		bool intersect = LineSegmentVsTriangle(test[i].line, test[i].tri, &point);
		if (intersect)
		{
			printf("\tIntersects at (%f, %f, %f)\n", point.x, point.y, point.z);
		}
		else
		{
			printf("\tMiss\n");
		}
		bool isOk = intersect == test[i].intersect;
		if (false == isOk)
		{
			printf("!!!WRONG\n");
		}
		allOk &= isOk;
	}
	
	return allOk ? 0 : -1;		// return -1 if any test has failed
}
