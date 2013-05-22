//-------------------------------------------------------------------
// File: plane.cpp
// 
// Infinite plane scene object implementation
//
//  
//-------------------------------------------------------------------

#include "illumination/material.h"

#define TILE_SIZE 16

#include "plane.h"

Plane::Plane(const Vec3D& normal, float D, Mtrl* material)
	: mNormal(normal),
		mD(D),
		mMtrl(material),
		mIsLight(false)
{
	mUAxis = Vec3D(mNormal.y(), mNormal.z(), -mNormal.x());
	mVAxis = cross(mUAxis, mNormal);
}

Plane::~Plane()
{
	// Plane owns material
	delete mMtrl;
}


CIsect Plane::intersect(const Ray& ray)
{
	float angle = dot(mNormal, ray.getDir());
	if (fabs(angle) < FLOAT_ZERO) 
	{
		return CIsect(false);
	}
	float t = (-(dot(ray.getOrg(), mNormal) + mD) / angle);
	if (t > 0.f)
	{
		CIsect res(true, t, this, getNormal(ray, t));
		res.Dst.push_back(t);
		res.InsideIntervals.push_back(Span(t, t));
		return res;
	}
	return CIsect(false);
}

Color Plane::getAmbColor(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	return mMtrl->AmbColor;
}

Color Plane::getDifColor(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	if (mMtrl->DifTexture)
	{
		Vec3D texCoords = getTexCoords(pnt, isect);
		return scale3D(mMtrl->DifTexture->sample(texCoords.x(), texCoords.y()), mMtrl->DifColor);
	}
	return mMtrl->DifColor;
}

Color Plane::getSpcColor(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	return mMtrl->SpcColor;
}

Vec3D Plane::getTexCoords(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	// That's not really good to calculate coordinates according to set texture, but plane if infinite and so on..

	const float xu = dot(pnt, mUAxis) * mMtrl->TexScaleU;
	const float yv = dot(pnt, mVAxis) * mMtrl->TexScaleV; 

	//float u = (static_cast<int>(xu) % TILE_SIZE + (xu - floorf(xu))) * 1.f / TILE_SIZE; 
	//float v = (static_cast<int>(yv) % TILE_SIZE + (yv - floorf(yv))) * 1.f / TILE_SIZE;
	//// Repeat texture around negative axis
	//if (u < 0.f)
	//{
	//	u = 1.f + u;
	//}
	//if (v < 0.f)
	//{
	//	v = 1.f + v; 
	//}
	return Vec3D(xu, yv, 0.f);
}