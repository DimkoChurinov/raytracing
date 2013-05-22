#ifndef GEOMETRY_TRIANGLE_H
#define GEOMETRY_TRIANGLE_H

#include "interfaces/ishape.h"
	
class Triangle : public IShape
{
public:
	explicit Triangle(const Vec3D& v0, const Vec3D& v1, const Vec3D& v2, Mtrl* material)
	: mV0(v0),
		mV1(v1),
		mV2(v2),
		mMtrl(material),
		mIsLight(false)
	{
		Vec3D e1 = v1 - v0;
		Vec3D e2 = v2 - v0;
		mNormal = cross(e1, e2).toUnit();
	}
	virtual ~Triangle();
	virtual CIsect intersect(const Ray& ray);
	virtual const Mtrl* getMtrl() const
	{
		return mMtrl;
	}

	virtual Vec3D getNormal(const Ray&, float, const CIsect& isect = CIsect()) const
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
	Vec3D mV0, mV1, mV2, mNormal;		
	Mtrl *mMtrl;
	bool mIsLight;
};

#endif