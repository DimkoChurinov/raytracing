//-------------------------------------------------------------------
// File: cylinder.cpp
// 
// Cylinder scene object implementation
//
//  
//-------------------------------------------------------------------

#define _USE_MATH_DEFINES
#include <math.h>

#include "illumination/material.h"

#include "cylinder.h"

Cylinder::Cylinder(const Vec3D& top, const Vec3D& bottom, float radius, Mtrl* material)
  : mTop(top),
    mBottom(bottom),
    mRadius(radius),
    mMtrl(material),
		mIsLight(false)
{
  mAxis		 = (mTop - mBottom).toUnit();
	mRadius2 = mRadius * mRadius;
	mVe			 = Vec3D(mAxis.y(), mAxis.z(), -mAxis.x());
	mVn			 = cross(mVe, mAxis);
}

Cylinder::~Cylinder()
{
  delete mMtrl;
}

CIsect Cylinder::intersect(const Ray& ray)
{
  const Vec3D& origin    = ray.getOrg();
  const Vec3D& direction = ray.getDir();

	const Vec3D& CO = origin - mBottom;
	
	const Vec3D& u = direction - mAxis * (dot(direction, mAxis));
	const Vec3D& v = CO - mAxis * (dot(CO, mAxis));

	CIsect res = CIsect(true);
	// Let a, b and c be coefficients of some square equation
	const float a = dot(u, u);
	float root		= 0.f;
	float closest = -1.f;
	float rayExit = -1.f;
	if (fabs(a) > FLOAT_ZERO)
	{
		const float b = 2 * dot(u, v);
		const float c = dot(v, v) - mRadius2;

		float D = b * b - 4 * a * c;

		// Complete miss
		if (D < 0.f) 
		{
			return CIsect(false);
		}

		D = sqrtf(D);

		// Calculate roots and take closest
		float denom = 1 / (2 * a);

		root = (-b - D) * denom;
		if (root >= 0.f)
		{
			Vec3D toBottom = ray.apply(root) - mBottom;
			Vec3D toTop		= ray.apply(root) - mTop;
			if (dot(mAxis, toBottom) > 0.f && dot(mAxis, toTop) < 0.f)
			{
				res.Dst.push_back(root);
				closest = root;
			}
		}
		root = (-b + D) * denom;
		if (root > 0.f)
		{
			// Awful copy paste :(
			Vec3D toBottom = ray.apply(root) - mBottom;
			Vec3D toTop		= ray.apply(root) - mTop;
			if (dot(mAxis, toBottom) > 0.f && dot(mAxis, toTop) < 0.f)
			{
				res.Dst.push_back(root);
				if (closest < 0.f)
				{
					root = closest;
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
		}
	}
	
	// dot(va, (q - p1)) = 0   t = (dot(va, p1) - dot(va, p)) / dot(va, v)

	// Find intersection with caps
	// Bottom one
	float axisToDir = dot(mAxis, direction);

	if (fabs(axisToDir) < FLOAT_ZERO)
	{
		if (closest > 0.f)
		{
			res.Distance = closest;
			res.Object   = this;
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

	float axisToOrg		= dot(mAxis, origin);
	//root = (dot(mAxis, mBottom) - axisToOrg) / axisToDir;
	float CODotAxis = dot(CO, mAxis);
	root = -CODotAxis / axisToDir;
	if (root > 0.f)
	{
		Vec3D toBottom = ray.apply(root) - mBottom;
		if (dot(toBottom, toBottom) < mRadius2)
		{
			res.Dst.push_back(root);
			// Awful copy paste :(
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
	}
	// Top one
	//root = (dot(mAxis, mTop) - axisToOrg) / axisToDir;
	float CTDotAxis = dot(origin - mTop, -mAxis);
	root = CTDotAxis / axisToDir;
	if (root > 0.f)
	{
		// Awful copy paste :(
		Vec3D toTop = ray.apply(root) - mTop;
		if (dot(toTop, toTop) < mRadius2)
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
	}
	
	if (closest >= 0.f)
	{
		res.Distance = closest;
		res.Object   = this;
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

Vec3D Cylinder::getNormal(const Ray& ray, float distance, const CIsect& isect /*= CIsect()*/) const
{
	const Vec3D atSurface = ray.apply(distance);
  // Check, whether pnt is lying on caps
	// Bottom
	const Vec3D& toBottom = atSurface - mBottom;
	if (fabs(dot(mAxis, toBottom)) < FLOAT_ZERO && dot(toBottom, toBottom) < mRadius2)
	{
		return -mAxis;
	}
	const Vec3D& toTop = atSurface - mTop;
	if (fabs(dot(mAxis, toTop)) < FLOAT_ZERO && dot(toTop, toTop) < mRadius2)
	{
		return mAxis;
	}
    
	return (atSurface - mAxis * dot(toBottom, mAxis) - mBottom).toUnit();
}

Color Cylinder::getAmbColor(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	return mMtrl->AmbColor;
}

Color Cylinder::getDifColor(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	if (mMtrl->DifTexture)
	{
		Vec3D texCoords = getTexCoords(pnt, isect);
		return scale3D(mMtrl->DifTexture->sample(texCoords.x(), texCoords.y()), mMtrl->DifColor);
	}
	return mMtrl->DifColor;
}

Color Cylinder::getSpcColor(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	return mMtrl->SpcColor;
}

Vec3D Cylinder::getTexCoords(const Vec3D& pnt, const CIsect& isect /*= CIsect()*/) const
{
	// Get position of pnt on the surface of the cylinder
	const Vec3D CO = pnt - mBottom;

	float CODotAxis = dot(CO, mAxis);

	Vec3D atCircle = pnt - CODotAxis * mAxis - mBottom;
	float		 x			  = dot(atCircle, mVe);
	float		 z				= dot(atCircle, mVn);

	float u = atan2(x, z) / (2 * M_PI);

	// v coordinate is the projection of the pnt on axis, mapped to [0..1]
	const float height = length(mTop - mBottom);

	float v = CODotAxis / height;

	return Vec3D(u, v, 0.f);
}