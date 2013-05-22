#ifndef GEOMETRY_BBOX_H
#define GEOMETRY_BBOX_H

#include "geometry/vector3D.h"

class Ray;

struct BBox
{
	bool intersect(const Ray& ray) const;
	Vec3D Max, Min;
};

#endif