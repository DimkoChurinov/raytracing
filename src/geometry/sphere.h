#ifndef GEOMETRY_SPHERE_H
#define GEOMETRY_SPHERE_H
	
#include "interfaces/ishape.h"

#include "geometry/ray.h"
#include "geometry/vector3d.h"

class Sphere : public IShape
{
public:
	Sphere(const Vec3D& center, float radius, Mtrl* material);
  virtual ~Sphere();
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
	Vec3D	mCenter, mVn, mVe, mVc;
	float mRadius, mRadius2;
	
	Mtrl* mMtrl;
	bool	mIsLight;
};

#endif