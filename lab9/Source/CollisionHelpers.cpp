#include "ITPEnginePCH.h"

namespace Collision
{

	bool Intersects(const Sphere& a, const Sphere& b)
	{
		Vector3 diff = a.mCenter - b.mCenter;
		float DistSq = diff.LengthSq();
		float sumRadiiSq = (a.mRadius + b.mRadius) * (a.mRadius + b.mRadius);
		if (DistSq <= sumRadiiSq)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool Intersects(const AxisAlignedBox & a, const AxisAlignedBox & b)
	{
		if (a.mMin.x > b.mMax.x) {
			return false;
		}

		if (a.mMax.x < b.mMin.x) {
			return false;
		}

		if (a.mMin.y > b.mMax.y) {
			return false;
		}

		if (a.mMax.y < b.mMin.y) {
			return false;
		}

		if (a.mMin.z > b.mMax.z) {
			return false;
		}

		if (a.mMax.z < b.mMin.z) {
			return false;
		}
		
		return true;
	}

	bool SegmentCast(const LineSegment& segment, const AxisAlignedBox& box, Vector3& outPoint)
	{
		float tmin = 0.0f;
		float tmax = FLT_MAX;
		Vector3 d = segment.mEnd - segment.mStart;
		Vector3 p = segment.mStart;

		if (fabsf(d.x) < 0.001f) {
			if (p.x < box.mMin.x || p.x > box.mMax.x) {
				return false;
			}
		}
		else {
			float oodx = 1.0f / d.x;
			float t1 = (box.mMin.x - p.x) * oodx;
			float t2 = (box.mMax.x - p.x) * oodx;

			if (t1 > t2) {
				float temp = t1;
				t1 = t2;
				t2 = temp;
			}

			tmin = Math::Max(tmin, t1);
			tmax = Math::Min(tmax, t2);

			if (tmin > tmax) {
				return false;
			}
		}

		if (fabsf(d.y) < 0.001f) {
			if (p.y < box.mMin.y || p.y > box.mMax.y) {
				return false;
			}
		}
		else {
			float oody = 1.0f / d.y;
			float t3 = (box.mMin.y - p.y) * oody;
			float t4 = (box.mMax.y - p.y) * oody;

			if (t3 > t4) {
				float temp = t3;
				t3 = t4;
				t4 = temp;
			}

			tmin = Math::Max(tmin, t3);
			tmax = Math::Min(tmax, t4);

			if (tmin > tmax) {
				return false;
			}
		}

		if (fabsf(d.z) < 0.001f) {
			if (p.z < box.mMin.z || p.z > box.mMax.z) {
				return false;
			}
		}
		else {
			float oodz = 1.0f / d.z;
			float t5 = (box.mMin.z - p.z) * oodz;
			float t6 = (box.mMax.z - p.z) * oodz;

			if (t5 > t6) {
				float temp = t5;
				t5 = t6;
				t6 = temp;
			}

			tmin = Math::Max(tmin, t5);
			tmax = Math::Min(tmax, t6);

			if (tmin > tmax) {
				return false;
			}
		}

		outPoint = p + d * tmin;
		return true;
	}

} // namespace
