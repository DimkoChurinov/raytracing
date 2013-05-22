#ifndef GEOMETRY_TEXTUREDTRIANGLE_H
#define GEOMETRY_TEXTUREDTRIANGLE_H

#include "triangle.h"

class TexturedTriangle : public Triangle
{
public:
	explicit TexturedTriangle(const Vec3D& v0, const Vec3D& v1, const Vec3D& v2, 
														const Vec3D& uv0, const Vec3D& uv1, const Vec3D& uv2,
														Mtrl* material)
		: Triangle(v0, v1, v2, material),
			mUV0(uv0),
			mUV1(uv1),
			mUV2(uv2)
	{
			
	}

	virtual Vec3D getTexCoords(const Vec3D& pnt, const CIsect& isect) const
	{
		return isect.U * mUV1 + isect.V * mUV2 + (1 - isect.U - isect.V) * mUV0;
	}

private:
 	Vec3D mUV0, mUV1, mUV2;
};

#endif 