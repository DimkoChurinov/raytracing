#ifndef GEOMETRY_BOX_H
#define GEOMETRY_BOX_H

#include "interfaces/ishape.h"

#include "geometry/ray.h"
#include "geometry/vector3d.h"

class Box : public IShape
{
public:
  explicit Box(const Vec3D& min, const Vec3D& max, Mtrl* material);
  virtual ~Box();
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
  Vec3D	mMin, mMax;
  float	mDiagonalLength;
  Mtrl* mMtrl;
  bool mIsLight;
};

#endif