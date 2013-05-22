#ifndef GEOMETRY_MODEL_H
#define GEOMETRY_MODEL_H

#include <vector>
#include "interfaces/ishape.h"
#include "geometry/bbox.h"
#include "geometry/modeltriangle.h"
#include "geometry/ray.h"
#include "geometry/vector3d.h"

class Triangle;

class Model : public IShape
{
public:
	Model(const std::vector< ModelTriangle* >& triangles, const BBox& bbox, Mtrl* material);
	virtual ~Model();
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
	std::vector< ModelTriangle* > mTriangles;
	BBox													mBoundingBox;
	Mtrl* mMtrl;
	bool			mIsLight;
};

#endif