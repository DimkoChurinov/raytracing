//-------------------------------------------------------------------
// File: torus.cpp
// 
// Torus scene object implementation
//
//  
//-------------------------------------------------------------------

#define _USE_MATH_DEFINES
#include <assert.h>
#include <math.h>

#include <algorithm>
#include <vector>

#include "illumination/material.h"
#include "vendors/quarticsolver.h"

#include "torus.h"

Torus::Torus(const Vec3D& center, const Vec3D& axis, float innerRadius, float outerRadius, Mtrl* material)
	: mCenter(center),
		mAxis(axis),
		mInnerRadius(innerRadius),
		mOuterRadius(outerRadius),
		mMtrl(material),
		mIsLight(false)
{
	mInnerRadius2 = mInnerRadius * mInnerRadius;
	mOuterRadius2	= mOuterRadius * mOuterRadius;
}

Torus::~Torus()
{
	delete mMtrl;
}

CIsect Torus::intersect(const Ray& ray)
{
	const Vec3D& rayOrg    = ray.getOrg();
	const Vec3D& rayDir = ray.getDir();

	const Vec3D CO	 = rayOrg - mCenter;

	const float CODotDir   = dot(rayDir, CO);

	const float		 CO2 = dot(CO, CO);
	const float		 u	 = dot(mAxis, CO);
	const float		 v   = dot(mAxis, rayDir);
	const float		 a   = (1 - v * v); // At general (dot(rayDir, rayDir) - v * v), but ray direction is normalized)
	const float		 b   = 2 * (dot(CO, rayDir) - u * v);
	const float    c   = (CO2 - u * u);
	const float		 d   = (CO2 + mOuterRadius2 - mInnerRadius2);

	//// Let A, B, C, D, E be the coefficients of general quadratic
	const float A = 1; // As soon as ray direction was normalized A = (P1 * P1)^2
	const float B = 4 * CODotDir;
	const float C = 2 * d + B * B * 0.25f - 4 * mOuterRadius2 * a;
	const float D = B * d - 4 * mOuterRadius2 * b;
	const float E = d * d - 4 * mOuterRadius2 * c;

	// Now solve quadratic, extern solver requires double array as result for roots
	const int cRootsCount = 4;
	double solutions[cRootsCount]; // 4 is the maximum of roots

	QuarticEquation equation(A, B, C, D, E);
	int rootsCount = equation.Solve(solutions);

	if (rootsCount == 0)
	{
		return CIsect(false);
	}

	// Sort found roots and get the lowest positive solution
	std::sort(solutions, solutions + cRootsCount);

	int					 closest = 0;
	CIsect res(true);
	// Find first positive solution in sorted array
	for (int idx = 0; idx < cRootsCount; ++idx)
	{
		const float t = solutions[idx];
		if (t > 0.f)
		{
			closest = idx;
			break;
		}
	}

	// Store all intersections
	if (closest < cRootsCount && closest > 0.f)
	{
		for (int idx = closest; idx < cRootsCount; ++idx)
		{
			res.Dst.push_back(solutions[idx]);
		}

		res.Distance = solutions[closest];
		res.Object	 = this;
		res.Normal	 = getNormal(ray, solutions[closest]);

		return res;
	}

	return CIsect(false);
}


Vec3D Torus::getNormal(const Ray& ray, float distance, const CIsect& isect /*= CIsect()*/) const
{
	const Vec3D atSurface = ray.apply(distance);
	const float		 y = dot((atSurface - mCenter), mAxis);
	const Vec3D D = ((atSurface - mCenter) - y * mAxis).toUnit();
	const Vec3D X = D * mOuterRadius;
	return (atSurface - mCenter + X).toUnit();
}

Color Torus::getAmbColor(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	return mMtrl->AmbColor;
}

Color Torus::getDifColor(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	if (mMtrl->DifTexture)
	{
		Vec3D texCoords = getTexCoords(pnt, isect);
		return scale3D(mMtrl->DifTexture->sample(texCoords.x(), texCoords.y()), mMtrl->DifColor);
	}
	return mMtrl->DifColor;
}

Color Torus::getSpcColor(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	return mMtrl->SpcColor;
}

Vec3D Torus::getTexCoords(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	// TODO:
	return Vec3D();
}
