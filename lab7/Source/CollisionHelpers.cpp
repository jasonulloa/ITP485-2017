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
		if ((a.mMax.x < b.mMin.x)
			|| (a.mMax.y < b.mMin.y)
			|| (a.mMax.z < b.mMin.z)
			|| (a.mMin.x > b.mMax.x)
			|| (a.mMin.y > b.mMax.y)
			|| (a.mMin.z > b.mMax.z)
			)
			return false;
			// TODO Lab 5f
		return true;
	}

	bool SegmentCast(const LineSegment& segment, const AxisAlignedBox& box, Vector3& outPoint)
	{
		// TODO Lab 5c
		static const float s_closeEnuf = 0.001f;
		Vector3 d = segment.mEnd - segment.mStart;
		float tmin = -FLT_MAX; // set to -FLT_MAX to get first hit on line
		float tmax = FLT_MAX; // set to max distance ray can travel (for segment)

							  // x axis
		if (fabsf(d.x) < s_closeEnuf)
		{
			// Ray is parallel to slab. No hit if origin not within slab
			if (segment.mStart.x < box.mMin.x || segment.mStart.x > box.mMax.x)
				return false;
		}
		else
		{
			float ood = 1.0f / d.x;
			float t1 = (box.mMin.x - segment.mStart.x) * ood;
			float t2 = (box.mMax.x - segment.mStart.x) * ood;
			// Make t1 be intersection with near plane, t2 with far plane
			if (t1 > t2)
			{
				float temp = t2;
				t2 = t1;
				t1 = t2;
			}
			// Compute the intersection of slab intersection intervals
			tmin = max(tmin, t1); // Rather than: if (t1 > tmin) tmin = t1;
			tmax = min(tmax, t2); // Rather than: if (t2 < tmax) tmax = t2;
								  // Exit with no collision as soon as slab intersection becomes empty
			if (tmin > tmax)
				return false;
		}

		// y axis
		if (fabsf(d.y) < s_closeEnuf)
		{
			// Ray is parallel to slab. No hit if origin not within slab
			if (segment.mStart.y < box.mMin.y || segment.mStart.y > box.mMax.y)
				return false;
		}
		else
		{
			float ood = 1.0f / d.y;
			float t1 = (box.mMin.y - segment.mStart.y) * ood;
			float t2 = (box.mMax.y - segment.mStart.y) * ood;
			// Make t1 be intersection with near plane, t2 with far plane
			if (t1 > t2)
			{
				float temp = t2;
				t2 = t1;
				t1 = t2;
			}
			// Compute the intersection of slab intersection intervals
			tmin = max(tmin, t1); // Rather than: if (t1 > tmin) tmin = t1;
			tmax = min(tmax, t2); // Rather than: if (t2 < tmax) tmax = t2;
								  // Exit with no collision as soon as slab intersection becomes empty
			if (tmin > tmax)
				return false;
		}

		// z axis
		if (fabsf(d.z) < s_closeEnuf)
		{
			// Ray is parallel to slab. No hit if origin not within slab
			if (segment.mStart.z < box.mMin.z || segment.mStart.z > box.mMax.z)
				return false;
		}
		else
		{
			float ood = 1.0f / d.z;
			float t1 = (box.mMin.z - segment.mStart.z) * ood;
			float t2 = (box.mMax.z - segment.mStart.z) * ood;
			// Make t1 be intersection with near plane, t2 with far plane
			if (t1 > t2)
			{
				float temp = t2;
				t2 = t1;
				t1 = t2;
			}
			// Compute the intersection of slab intersection intervals
			tmin = max(tmin, t1); // Rather than: if (t1 > tmin) tmin = t1;
			tmax = min(tmax, t2); // Rather than: if (t2 < tmax) tmax = t2;
								  // Exit with no collision as soon as slab intersection becomes empty
			if (tmin > tmax)
				return false;
		}

		if (tmin > 1.0f || tmax < 0.0f)
			return false;

		// Ray intersects all 3 slabs. Return point (q) and intersection t value (tmin)
		outPoint = segment.mStart + d * tmin;
		return true;
	}

} // namespace
