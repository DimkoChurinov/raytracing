#ifndef GEOMETRY_INTERSECTION_H
#define GEOMETRY_INTERSECTION_H

#include <vector>
	
#include "span.h"
#include "vector3d.h"

struct IShape;

struct CIsect
{
	explicit CIsect(bool hit = false, float dist = -1.f, IShape *object = 0x0, const Vec3D& normal = Vec3D())
		: Exists(hit),
			Distance(dist),
			Object(object),
			Normal(normal)
	{
	}

	bool		 Exists;
	IShape  *Object;
	Vec3D Normal, TexCoords;
	float	U, V, Distance;
  std::vector< float > Dst;
	Spans InsideIntervals;
};

#endif