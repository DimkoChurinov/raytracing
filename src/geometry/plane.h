#ifndef GEOMETRY_PLANE_H
#define GEOMETRY_PLANE_H

#include "interfaces/ishape.h"

class Plane : public IShape
{
public:
	explicit Plane(const Vec3D& normal, float D, Mtrl* material);
  virtual ~Plane();
	virtual CIsect intersect(const Ray& ray);
	virtual const Mtrl* getMtrl() const
	{
		return mMtrl;
	}
	virtual Vec3D getNormal(const Ray& ray, float distance, const CIsect& isect = CIsect()) const
	{
		return mNormal;
	}
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
	Vec3D	mNormal, mUAxis, mVAxis;
	float	mD;
	Mtrl *mMtrl;
	bool mIsLight;
};

#endif
