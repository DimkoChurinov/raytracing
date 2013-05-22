//-------------------------------------------------------------------
// File: cone.cpp
// 
// Cone scene object implementation
//
//  
//-------------------------------------------------------------------

#include "illumination/material.h"

#include "cone.h"

Cone::Cone(const Vec3D& top, const Vec3D& bottom, float radius, Mtrl* material)
	: mTop(top),
		mBottom(bottom),
		mRadius(radius),
		mMtrl(material),
		mIsLight(false)
{
	mAxis					= (mTop - mBottom).toUnit();
	mRadius2			= mRadius * mRadius;
	mRadPerHeight = mRadius / length(mTop - mBottom);
}

Cone::~Cone()
{
	delete mMtrl;
}

CIsect Cone::intersect(const Ray& ray)
{
	const Vec3D& rayOrg    = ray.getOrg();
	const Vec3D& rayDir = ray.getDir();

	const Vec3D& CO = rayOrg - mBottom;

	const float dirDotAxis = dot(rayDir, mAxis);
	const float CODotAxis	 = dot(CO, mAxis);

	const Vec3D& u = rayDir + mAxis * (-dirDotAxis);
	const Vec3D& v = CO + mAxis * (-CODotAxis);
	const float     w = CODotAxis * mRadPerHeight;

	const float	radPerDir = dirDotAxis * mRadPerHeight;

	CIsect res = CIsect(true);

	// Let a, b, c be the coefficients of the square equation
	const float a = dot(u, u) - radPerDir * radPerDir;
	float closest = -1.f;
	float root		= 0.f;
	float rayExit = -1.f; // Distance, where ray has exited the cone
	if (fabs(a) > FLOAT_ZERO)
	{
		const float b = 2 * (dot(u, v) - w * radPerDir);
		const float c = dot(v, v) - w * w;

		float D = b * b - 4 * a * c;

		// Complete miss
		if (D < 0.f) 
		{
			return CIsect(false);
		}

		D = sqrtf(D);

		const float denom = 1 / (2 * a);

		root = (-b - D) * denom;
		if (root > 0.f)
		{
			const Vec3D& surfacePoint = ray.apply(root);
			Vec3D toBottom = surfacePoint - mBottom;
			Vec3D toTop		= surfacePoint - mTop;
			if (dot(mAxis, toBottom) > 0.f && dot((-mAxis), toTop) > 0.f)
			{
				res.Dst.push_back(root);
				closest = root;
			}
		}
		root = (-b + D) * denom;
		if (root > 0.f)
		{
			const Vec3D& surfacePoint = ray.apply(root);
			Vec3D toBottom = surfacePoint - mBottom;
			Vec3D toTop		= surfacePoint - mTop;
			if (dot(mAxis, toBottom) > 0.f && dot(-mAxis, toTop) > 0.f)
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
	}

	// CIsect with bottom cap

	if (fabs(dirDotAxis) < FLOAT_ZERO)
	{
		if (closest > 0.f)
		{
			res.Exists	 = true;
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

	// Somewhere I messed up with the top and bottom pnts
	root = (dot(-mAxis, rayOrg - mTop)) / dirDotAxis;
	if (root > 0.f)
	{
		Vec3D test = ray.apply(root) - mTop;
		if (dot(test, test) < mRadius2)
		{
			res.Dst.push_back(root);
			if (closest < 0.f)
			{
				closest = root;
				rayExit = root;
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
	
	if (closest > 0.f)
	{
			res.Exists	 = true;
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


Vec3D Cone::getNormal(const Ray& ray, float distance, const CIsect& isect /*= CIsect()*/) const
{
	const Vec3D atSurface = ray.apply(distance);
	// Check, whether pnt is lying on caps
	// Bottom
	const Vec3D& toBottom		 = atSurface - mTop;
	if (fabs(dot(mAxis, toBottom)) < FLOAT_ZERO && dot(toBottom, toBottom) < mRadius2)
	{
		return mAxis;
	}
	
	const Vec3D& approxNorm = atSurface - (mAxis * (dot(atSurface - mTop, mAxis)) + mTop); // Check me
	return (approxNorm + mAxis * (-mRadPerHeight * length(approxNorm))).toUnit();
}

Color Cone::getAmbColor(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	return mMtrl->AmbColor;
}

Color Cone::getDifColor(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	if (mMtrl->DifTexture)
	{
		Vec3D texCoords = getTexCoords(pnt, isect);
		return mMtrl->DifTexture->sample(texCoords.x(), texCoords.y());
	}
	return mMtrl->DifColor;
}

Color Cone::getSpcColor(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	return mMtrl->SpcColor;
}

Vec3D Cone::getTexCoords(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	// TODO:
	return Vec3D();
}