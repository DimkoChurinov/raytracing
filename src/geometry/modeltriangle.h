#ifndef GEOMETRY_MODELTRIANGLE_H
#define GEOMETRY_MODELTRIANGLE_H

#include "smoothtriangle.h"
#include "texturedtriangle.h"
	
class ModelTriangle : virtual public SmoothTriangle,
											virtual public TexturedTriangle
{
public:
  explicit ModelTriangle(const Vec3D& v0,  const Vec3D& v1,  const Vec3D& v2, 
													const Vec3D& n0,  const Vec3D& n1,  const Vec3D& n2,
													const Vec3D& uv0, const Vec3D& uv1, const Vec3D& uv2,
													Mtrl* material)
		: SmoothTriangle(v0, v1, v2, n0, n1, n2, material),
			TexturedTriangle(v0, v1, v2, uv0, uv1, uv2, material)
	{
			
	}

	virtual CIsect intersect(const Ray& ray)
	{
		return Triangle::intersect(ray);
	}
};

#endif
