//-------------------------------------------------------------------
// File: triangle.h
// 
// Triangle scene object implementation
//			 Algorithm can be found here, so don't get confused with identifier names
//			 http://geomalgorithms.com/a06-_intersect-2.html
//
//  
//-------------------------------------------------------------------

#include "illumination/material.h"

#include "triangle.h"
	

Triangle::~Triangle()
{
	delete mMtrl;
}

CIsect Triangle::intersect(const Ray& ray)
{
	const Vec3D& origin	  = ray.getOrg();
	const Vec3D& direction = ray.getDir();

	const Vec3D e1 = mV1 - mV0;
	const Vec3D e2 = mV2 - mV0;

	const Vec3D pvec = cross(direction, e2);
	const float		 det  = dot(e1, pvec);

	if (fabs(det) < FLOAT_ZERO)
	{
		return CIsect(false);
	}

	const float invDet = 1.f / det;

	const Vec3D tvec		= origin - mV0;
	float					 lambda = dot(tvec, pvec);

	lambda *= invDet;

	if (lambda < 0.f || lambda > 1.f)
	{
		return CIsect(false);
	}

	const Vec3D qvec = cross(tvec, e1);
	float					 mue	= dot(direction, qvec);

	mue *= invDet;

	if (mue < 0.f || mue + lambda > 1.f)
	{
		return CIsect(false);
	}

	float f = dot(e2, qvec);
	f = f * invDet - FLOAT_ZERO;

	if (f < FLOAT_ZERO)
	{
		return CIsect(false);
	}

	CIsect isect(true, f, this);
	
	isect.Dst.push_back(f);

	isect.U = lambda;
	isect.V = mue;
	isect.Normal    = getNormal(ray, f, isect); // CIsect data may be needed in subclasses
  isect.TexCoords = getTexCoords(ray.apply(f), isect);

	return isect;
}

Color Triangle::getAmbColor(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	return mMtrl->AmbColor;
}

Color Triangle::getDifColor(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	if (mMtrl->DifTexture)
	{
		Vec3D texCoords = getTexCoords(pnt, isect);
		return scale3D(mMtrl->DifTexture->sample(texCoords.x(), texCoords.y()), mMtrl->DifColor);
	}
	return mMtrl->DifColor;
}

Color Triangle::getSpcColor(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	return mMtrl->SpcColor;
}

Vec3D Triangle::getTexCoords(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	return isect.U * mV1 + isect.V * mV2 + (1 - isect.U - isect.V) * mV0;
}
