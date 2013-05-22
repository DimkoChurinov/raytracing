#ifndef GEOMETRY_SMOOTHTRIANGLE_H
#define GEOMETRY_SMOOTHTRIANGLE_H

#include "triangle.h"

class SmoothTriangle : public Triangle
{
public:
	explicit SmoothTriangle(const Vec3D& v0, const Vec3D& v1, const Vec3D& v2, 
													const Vec3D& n0, const Vec3D& n1, const Vec3D& n2,
													Mtrl* material)
		: Triangle(v0, v1, v2, material),
			mNormalV0(n0),
			mNormalV1(n1),
			mNormalV2(n2)
	{
			
	}

	virtual Vec3D getNormal(const Ray& ray, float distance, const CIsect& isect) const
	{
		Vec3D normal = isect.U * mNormalV1 + isect.V * mNormalV2 + (1 - isect.U - isect.V) * mNormalV0;
		return normal.toUnit();
	}

private:
	Vec3D mNormalV0, mNormalV1, mNormalV2;
};

#endif