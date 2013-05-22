//-------------------------------------------------------------------
// File: csgunion.cpp
// 
// Constructive solid geometry evaluation tree union operation
// Left and r nodes can be either values or operations.
//
//  
//-------------------------------------------------------------------

#include <algorithm>
#include <xutility>

#include "csgunion.h"

#define TOO_FAR_AWAY		 1000000.f 

CSGUnion::CSGUnion(CSGNode *lh, CSGNode *rh)
	: CSGOperation(lh, rh)
{
}

CSGUnion::~CSGUnion()
{
}

CIsect CSGUnion::intersect(const Ray& ray) 
{
	// Post-order depth-first tree traversal
	CIsect lIsect  = l()->intersect(ray);
	CIsect rIsect = r()->intersect(ray);

	// Update distance of not existing intersections to optimize algorithm
	if (!lIsect.Exists)
	{
		lIsect.Distance = TOO_FAR_AWAY;
	}
	if (!rIsect.Exists)
	{
		rIsect.Distance = TOO_FAR_AWAY;
	}

	CIsect unite;

	unite.Exists   = lIsect.Exists || rIsect.Exists;
	if (!unite.Exists)
	{
		return CIsect(false);
	}

	// We won't update intersected object, to be able to determine which object was really intersected
	if (lIsect.Distance < rIsect.Distance)
	{
		unite.Distance  = lIsect.Distance;
		unite.Normal	  = lIsect.Normal;
		unite.Object	  = lIsect.Object;
		unite.TexCoords = lIsect.TexCoords;
		unite.U					= lIsect.U;
		unite.V					= lIsect.V;
		// Do not copy distances - later
	}
	else
	{
		unite.Distance  = rIsect.Distance;
		unite.Normal	  = rIsect.Normal;
		unite.Object	  = rIsect.Object;
		unite.TexCoords = rIsect.TexCoords;
		unite.U					= rIsect.U;
		unite.V					= rIsect.V;
	}
	
	// Copy all distances
	//unite.Dst.resize(lIsect.Dst.size() + rIsect.Dst.size()); n
	if (lIsect.Exists)
	{
		unite.Dst.resize(lIsect.Dst.size());
		std::copy(lIsect.Dst.begin(), lIsect.Dst.end(), unite.Dst.begin());
	}
	if (rIsect.Exists)
	{
		const int size = unite.Dst.size();
		unite.Dst.resize(unite.Dst.size() + rIsect.Dst.size());
		std::copy(rIsect.Dst.begin(), rIsect.Dst.end(), unite.Dst.begin() + size);
	}

	return unite;
}

const Mtrl* CSGUnion::getMtrl() const
{
	return NULL;
}

Vec3D CSGUnion::getNormal(const Ray& ray, float distance, const CIsect& isect/* = CIsect() */) const
{
	return isect.Normal;
}

void CSGUnion::setIsLight(bool)
{
	/** Can't be light **/
}

bool CSGUnion::isLight() const
{
	/** Can't be light **/
	return false;
}

Color CSGUnion::getAmbColor(const Vec3D& pnt, const CIsect& isect /*= CIsect()*/) const
{
	// Fallback to data, cached in intersection
	return isect.Object->getAmbColor(pnt, isect);
}


Color CSGUnion::getDifColor(const Vec3D& pnt, const CIsect& isect /*= CIsect()*/) const
{
	// Fallback to data, cached in intersection
	return isect.Object->getDifColor(pnt, isect);
}


Color CSGUnion::getSpcColor(const Vec3D& pnt, const CIsect& isect /*= CIsect()*/) const
{
	// Fallback to data, cached in intersection
	return isect.Object->getSpcColor(pnt, isect);
}

Vec3D CSGUnion::getTexCoords(const Vec3D& pnt, const CIsect& isect /*= CIsect()*/) const
{
	// Fallback to data, cached in intersection
	return isect.Object->getTexCoords(pnt);
}
