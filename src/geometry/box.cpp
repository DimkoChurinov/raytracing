//-------------------------------------------------------------------
// File: box.cpp
// 
// Box scene object implementation
//
//  
//-------------------------------------------------------------------

#define _USE_MATH_DEFINES

#include <assert.h>
#include <math.h>
#include <cfloat>
#include <utility>
#include <xutility>

#include "illumination/material.h"

#include "box.h"

#define DIRECTION_EPSILON 0.1f
#define Infinity FLT_MAX

Box::Box(const Vec3D& min, const Vec3D& max, Mtrl* material)
  : mMin(min),
    mMax(max),
		mIsLight(false),
		mMtrl(material)
{
	mDiagonalLength = length(mMax - mMin);
}

Box::~Box()
{
  delete mMtrl;
}

CIsect Box::intersect(const Ray& ray)
{
  const Vec3D& origin    = ray.getOrg();
	const Vec3D& direction = ray.getDir();

	float tmax = Infinity, tmin = -Infinity;

	if (fabs(direction.x()) < FLOAT_ZERO)
	{
		int a = 0;
		a= 5;
	}

	float invDir = 0.f;
	if (fabs(direction.x()) > FLOAT_ZERO)
	{
		invDir = 1.f / direction.x();
		tmin = (mMin.x() - origin.x()) * invDir;
		tmax = (mMax.x() - origin.x()) * invDir;

		if (tmin > tmax)
		{
			std::swap(tmin, tmax);
		}
	}

	if (fabs(direction.y()) > FLOAT_ZERO)
	{
		invDir = 1.f / direction.y();
		float tymin = (mMin.y() - origin.y()) * invDir;
		float tymax = (mMax.y() - origin.y()) * invDir;

		if (tymin > tymax)
		{
			std::swap(tymin, tymax);
		}

		if (tymin > tmin)
		{
			tmin = tymin;
		}
		if (tymax < tmax)
		{
			tmax = tymax;
		}

		if (tmin > tmax)
		{
			return CIsect(false);
		}

		if (tmax < 0.f)
		{
			return CIsect(false);
		}
	}

	if (fabs(direction.z()) > FLOAT_ZERO)
	{
		invDir = 1.f / direction.z();
		float tzmin = (mMin.z() - origin.z()) * invDir;
		float tzmax = (mMax.z() - origin.z()) * invDir;

		if (tzmin > tzmax)
		{
			std::swap(tzmin, tzmax);
		}

		if (tzmin > tmin)
		{
			tmin = tzmin;
		}
		if (tzmax < tmax)
		{
			tmax = tzmax;
		}

		if (tmin > tmax)
		{
			return CIsect(false);
		}
	}

	if (tmax < 0.f)
	{
		return CIsect(false);
	}

	if (tmin < 0.f)
	{
		return CIsect(false);
	}

	if (tmin == Infinity)
	{
		return CIsect(false);
	}

	CIsect res = CIsect(true, tmin, this, getNormal(ray, tmin));
	res.Dst.resize(2);
	res.Dst[0] = tmin;
	res.Dst[1] = tmax;
	res.InsideIntervals.push_back(Span(tmin, tmax));
	return res;
}

Vec3D Box::getNormal(const Ray& ray, float distance, const CIsect& isect /*= CIsect()*/) const
{
	const Vec3D atSurface = ray.apply(distance);
  
	const Vec3D toMin = atSurface - mMin;
	const Vec3D toMax = atSurface - mMax;

	if (fabs(toMin.x()) < EPSILON)
	{
		return Vec3D(-1.f, 0.f, 0.f);
	}
	if (fabs(toMax.x()) < EPSILON)
	{
		return Vec3D(1.f, 0.f, 0.f);
	}
	if (fabs(toMin.y()) < EPSILON)
	{
		return Vec3D(0.f, -1.f, 0.f);
	}
	if (fabs(toMax.y()) < EPSILON)
	{
		return Vec3D(0.f, 1.f, 0.f);
	}
	if (fabs(toMin.z()) < EPSILON)
	{
		return Vec3D(0.f, 0.f, -1.f);
	}
	if (fabs(toMax.z()) < EPSILON)
	{
		return Vec3D(0.f, 0.f, 1.f);
	}
	assert(false);
	return Vec3D(0.f, 0.f, 0.f);
}

Color Box::getAmbColor(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	return mMtrl->AmbColor;
}

Color Box::getDifColor(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	if (mMtrl->DifTexture)
	{
		Vec3D texCoords = getTexCoords(pnt, isect);
		return scale3D(mMtrl->DifTexture->sample(texCoords.x(), texCoords.y()), mMtrl->DifColor);
	}
	return mMtrl->DifColor;
}

Color Box::getSpcColor(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	return mMtrl->SpcColor;
}

Vec3D Box::getTexCoords(const Vec3D& pnt, const CIsect& isect /*= CIsect()*/) const
{
	// TODO:
	return Vec3D();
}