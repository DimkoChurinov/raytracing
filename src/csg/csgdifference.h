#ifndef CSG_CSGDIFFERENCE_H
#define CSG_CSGDIFFERENCE_H

#include "csgoperation.h"

class CSGDifference : public CSGOperation
{
public:
	explicit CSGDifference(CSGNode* lh, CSGNode* rh);
	virtual ~CSGDifference();
	virtual CIsect intersect(const Ray& ray);
	virtual const Mtrl* getMtrl() const;
	virtual Vec3D getNormal(const Ray& ray, float distance, const CIsect& isect = CIsect()) const;
	virtual void setIsLight(bool light);
	virtual bool isLight() const;
	virtual Color getAmbColor(const Vec3D& pnt, const CIsect& isect = CIsect()) const;
	virtual Color getDifColor(const Vec3D& pnt, const CIsect& isect = CIsect()) const;
	virtual Color getSpcColor(const Vec3D& pnt, const CIsect& isect = CIsect()) const;
	virtual Vec3D getTexCoords(const Vec3D& pnt, const CIsect& isect = CIsect()) const;
};

#endif