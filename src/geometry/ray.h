#ifndef GEOMETRY_RAY_H
#define GEOMETRY_RAY_H

#include "vector3d.h"

class Ray
{
public:
	Ray(const Vec3D& org = Vec3D(), const Vec3D& dir = Vec3D())
		: mOrg(org), 
			mDir(dir)
	{
		mDir.normalize();
	}

	const Vec3D& getOrg() const
	{
		return mOrg;
	}

	const Vec3D& getDir() const
	{
		return mDir;
	}

	Vec3D apply(float t) const
	{
		return mOrg + mDir * t;
	}

private:
	Vec3D mOrg;
	Vec3D mDir;
};

#endif