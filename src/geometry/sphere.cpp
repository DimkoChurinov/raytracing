//-------------------------------------------------------------------
// File: sphere.cpp
// 
// Sphere scene object implementation
//
//  
//-------------------------------------------------------------------

#define _USE_MATH_DEFINES
#include <math.h>

#include "illumination/material.h"

#include "sphere.h"

Sphere::Sphere(const Vec3D& center, float radius, Mtrl* material)
	: mCenter(center),
		mRadius(radius),
		mMtrl(material),
		mIsLight(false)
{
	mRadius2 = radius * radius;
	mVn = Vec3D(0.f, 1.f, 0.f);
	mVe = Vec3D(1.f, 0.f, 0.f);
	mVc = cross(mVn, mVe);
}

Sphere::~Sphere()
{
	delete mMtrl;
}

CIsect Sphere::intersect(const Ray& ray)
{
	// Solve square equation
	Vec3D CO = ray.getOrg() - mCenter;

	// Let p and q be coefficients of the square equation x^2 + p * x + q = 0
	float p = dot(ray.getDir(), CO);
	float q = dot(CO, CO) - mRadius2;

	// Let D be discriminant of the equation
	float D = p * p - q;

	if (D < 0)
	{
		return CIsect(false);
	}

	D = sqrt(D);

	float				 closest = -1.f;
	CIsect res(true);
	
	// Get closest intersection
	float root = -p - D;
	float rayExit = -1.f;
	if (root >= 0.f)
	{
		closest = root;
		res.Dst.push_back(root);
	}
	
	root = -p + D;
	if (root >= 0.f)
	{
		res.Dst.push_back(root);
		if (closest < 0.f)
		{
			closest = root;
		}
		else if (root < closest)
		{
			rayExit = closest;
			closest = root;
		}
		else
		{
			rayExit = root;
		}
	}

	if (closest > 0.f)
	{
		res.Distance = closest;
		res.Object	 = this;
		res.Normal	 = getNormal(ray, closest);
		if (rayExit < 0.f)
		{
			res.InsideIntervals.push_back(Span(0.f, closest));
			res.Dst.insert(res.Dst.begin(), 0.f);
		}
		else
		{
			res.InsideIntervals.push_back(Span(closest, rayExit));
		}
		return res;
	}

	return CIsect(false);
}

Vec3D Sphere::getNormal(const Ray& ray, float distance, const CIsect& isect /*= CIsect()*/) const
{
	Vec3D normal = (ray.apply(distance) - mCenter) / mRadius;
	normal.normalize();
	return normal;
}


Color Sphere::getAmbColor(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	return mMtrl->AmbColor;
}

Color Sphere::getDifColor(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	if (mMtrl->DifTexture)
	{
		Vec3D texCoords = getTexCoords(pnt, isect);
		return scale3D(mMtrl->DifTexture->sample(texCoords.x(), texCoords.y()), mMtrl->DifColor);
	}
	return mMtrl->DifColor;
}

Color Sphere::getSpcColor(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	return mMtrl->SpcColor;
}

Vec3D Sphere::getTexCoords(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	const Vec3D CO = (pnt - mCenter) / mRadius;

	const float phi   = acosf(-dot(CO, mVn));
	const float theta = (acosf(dot(mVe, CO)) / (sinf(phi))) * (2.f / M_PI);

	float u;

	if (dot(mVc, CO) >= 0.f)
	{
		u = (1.f - theta) / mMtrl->TexScaleU;
	}
	else
	{
		u = theta / mMtrl->TexScaleU;
	}

	float v = phi / mMtrl->TexScaleV * (1.f / M_PI);

	return Vec3D(u, v, 0.f);
}