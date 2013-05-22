#ifndef GEOMETRY_TORUS_H
#define GEOMETRY_TORUS_H

#include "interfaces/ishape.h"

#include "geometry/ray.h"
#include "geometry/vector3d.h"

class Torus : public IShape
{
public:
	explicit Torus(const Vec3D& center, const Vec3D& axis, float innerRadius, float outedRadius, Mtrl* material);
	virtual ~Torus();
	virtual CIsect intersect(const Ray& ray);
	virtual const Mtrl* getMtrl() const
	{
		return mMtrl;
	}
	virtual Vec3D getNormal(const Ray& ray, float distance, const CIsect& isect = CIsect()) const;
	virtual void setIsLight(bool light)
	{
		mIsLight = light;
	}
	virtual bool isLight() const
	{
		return mIsLight;
	}
	virtual Color getAmbColor(const Vec3D& pnt, const CIsect& isect = CIsect()) const;
	virtual Color getDifColor(const Vec3D& pnt, const CIsect& isect = CIsect()) const;
	virtual Color getSpcColor(const Vec3D& pnt, const CIsect& isect = CIsect()) const;
	virtual Vec3D getTexCoords(const Vec3D& pnt, const CIsect& isect = CIsect()) const;

private:
	Vec3D	mCenter, mAxis;
	float mInnerRadius, mOuterRadius, mInnerRadius2, mOuterRadius2;
	Mtrl* mMtrl;
	bool mIsLight;
};

#endif