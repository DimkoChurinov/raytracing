//-------------------------------------------------------------------
// File: csgintersection.cpp
// 
// Constructive solid geometry evaluation tree intersection operation
// Left and r nodes can be either values or operations.
//
//  
//-------------------------------------------------------------------

#include <algorithm>
#include <xutility>

#include "csgintersection.h"

#define TOO_FAR_AWAY		 1000000.f 

CSGCIsect::CSGCIsect(CSGNode *lh, CSGNode *rh)
	: CSGOperation(lh, rh)
{
}

CSGCIsect::~CSGCIsect()
{
}

CIsect CSGCIsect::intersect(const Ray& ray) 
{
	// Post-order depth-first tree traversal
	CIsect lIsect  = l()->intersect(ray);
	CIsect rIsect = r()->intersect(ray);

	// Due to intersection, if one doesn't exist, no intersection at all
	if (!lIsect.Exists || !rIsect.Exists)
	{
		return CIsect(false);
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

	if (lMin < rMin && lMax > rMin)
	{
		intersect.Distance  = rMin;
		intersect.Object	  = rIsect.Object;
		intersect.Normal    = rIsect.Normal;
		intersect.U					= rIsect.U;
		intersect.V					= rIsect.V;
		intersect.TexCoords = rIsect.TexCoords;
		intersect.Dst.push_back(rMin);
		intersect.Dst.push_back(std::min(lMax, rMax));
	}
	else if (rMin < lMin && rMax > lMin)
	{
		intersect.Distance  = lMin;
		intersect.Object	  = lIsect.Object;
		intersect.Normal    = lIsect.Normal;
		intersect.U					= lIsect.U;
		intersect.V					= lIsect.V;
		intersect.TexCoords = lIsect.TexCoords;
		intersect.Dst.push_back(lMin);
		intersect.Dst.push_back(std::min(lMax, rMax));
	}
	else
	{
		return CIsect(false);
	}
	
	
	// Copy all the distances
	/*intersect.Dst.resize(lIsect.Dst.size() + rIsect.Dst.size());
	std::copy(lIsect.Dst.begin(), lIsect.Dst.end(), intersect.Dst.begin());
	std::copy(rIsect.Dst.begin(), rIsect.Dst.end(), intersect.Dst.begin() + lIsect.Dst.size());*/

	return intersect;
}

const Mtrl* CSGCIsect::getMtrl() const
{
	return NULL;
}

Vec3D CSGCIsect::getNormal(const Ray& ray, float distance, const CIsect& isect/* = CIsect() */) const
{
	return isect.Normal;
}

void CSGCIsect::setIsLight(bool)
{
	/** Can't be light **/
}

bool CSGCIsect::isLight() const
{
	/** Can't be light **/
	return false;
}

Color CSGCIsect::getAmbColor(const Vec3D& pnt, const CIsect& isect /*= CIsect()*/) const
{
	// Fallback to data, cached in intersection
	return isect.Object->getAmbColor(pnt, isect);
}


Color CSGCIsect::getDifColor(const Vec3D& pnt, const CIsect& isect /*= CIsect()*/) const
{
	// Fallback to data, cached in intersection
	return isect.Object->getDifColor(pnt, isect);
}


Color CSGCIsect::getSpcColor(const Vec3D& pnt, const CIsect& isect /*= CIsect()*/) const
{
	// Fallback to data, cached in intersection
	return isect.Object->getSpcColor(pnt, isect);
}

Vec3D CSGCIsect::getTexCoords(const Vec3D& pnt, const CIsect& isect /*= CIsect()*/) const
{
	// Fallback to data, cached in intersection
	return isect.Object->getTexCoords(pnt);
}
