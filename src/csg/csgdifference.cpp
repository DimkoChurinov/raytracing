//-------------------------------------------------------------------
// File: csgdifference.cpp
// 
// Constructive solid geometry evaluation tree difference operation
// Left and r nodes can be either values or operations.
//
//  
//-------------------------------------------------------------------

#include <algorithm>
#include <xutility>

#include "csgdifference.h"

#define TOO_FAR_AWAY		 1000000.f 

CSGDifference::CSGDifference(CSGNode *lh, CSGNode *rh)
	: CSGOperation(lh, rh)
{
}

CSGDifference::~CSGDifference()
{
}

CIsect CSGDifference::intersect(const Ray& ray) 
{
	// Difference operation is LeftHand \ RightHand, obviously non-commutative

	// Post-order depth-first tree traversal
	CIsect lIsect  = l()->intersect(ray);
	CIsect rIsect = r()->intersect(ray);

	// Due to intersection, if one doesn't exist, no intersection at all
	if (!lIsect.Exists)
	{
		return CIsect(false);
	}

	// Right not intersected, so just l intersection can be returned
	if (!rIsect.Exists)
	{
		return lIsect;
	}

	// Get min and max intersections
	float lMin  = TOO_FAR_AWAY, lMax  = -1.f;
	float rMin = TOO_FAR_AWAY, rMax = -1.f;

	// Sort intersections
	std::sort(lIsect.Dst.begin(), lIsect.Dst.end());
	std::sort(rIsect.Dst.begin(), rIsect.Dst.end());

	lMin  = lIsect.Dst.front();  lMax  = lIsect.Dst.back();
	rMin = rIsect.Dst.front(); rMax = rIsect.Dst.back();

	CIsect intersect(true);

	// Right object is closer, that l, so they even don't overlap
	if (rMax < lMin)
	{
		return lIsect;
	}

	// Left object is closer, than r, so r object
	if (lMax < rMin)
	{
		return lIsect;
	}

	
	if (lMin < rMin)
	{
		intersect.Distance  = lMin;
		intersect.Object	  = lIsect.Object;
		intersect.Normal    = lIsect.Normal; 
		intersect.U					= lIsect.U;
		intersect.V					= lIsect.V;
		intersect.TexCoords = lIsect.TexCoords;
	}
	else if (rMax < lMax)
	{
		intersect.Distance  = rMax;
		intersect.Object	  = rIsect.Object;
		intersect.Normal    = -rIsect.Normal; // Use normal of negative object
		intersect.U					= lIsect.U;
		intersect.V					= lIsect.V;
		intersect.TexCoords = lIsect.TexCoords;
	}
	else 
	{
		return CIsect(false);
	}

	// Copy all the distances
	intersect.Dst.resize(lIsect.Dst.size() + rIsect.Dst.size());
	std::copy(lIsect.Dst.begin(), lIsect.Dst.end(), intersect.Dst.begin());
	std::copy(rIsect.Dst.begin(), rIsect.Dst.end(), intersect.Dst.begin() + lIsect.Dst.size());

	return intersect;
}

const Mtrl* CSGDifference::getMtrl() const
{
	return NULL;
}

Vec3D CSGDifference::getNormal(const Ray& ray, float distance, const CIsect& isect/* = CIsect() */) const
{
	return isect.Normal;
}

void CSGDifference::setIsLight(bool)
{
	/** Can't be light **/
}

bool CSGDifference::isLight() const
{
	/** Can't be light **/
	return false;
}

Color CSGDifference::getAmbColor(const Vec3D& pnt, const CIsect& isect /*= CIsect()*/) const
{
	// Fallback to data, cached in intersection
	return isect.Object->getAmbColor(pnt, isect);
}


Color CSGDifference::getDifColor(const Vec3D& pnt, const CIsect& isect /*= CIsect()*/) const
{
	// Fallback to data, cached in intersection
	return isect.Object->getDifColor(pnt, isect);
}


Color CSGDifference::getSpcColor(const Vec3D& pnt, const CIsect& isect /*= CIsect()*/) const
{
	// Fallback to data, cached in intersection
	return isect.Object->getSpcColor(pnt, isect);
}

Vec3D CSGDifference::getTexCoords(const Vec3D& pnt, const CIsect& isect /*= CIsect()*/) const
{
	// Fallback to data, cached in intersection
	return isect.Object->getTexCoords(pnt);
}
