//-------------------------------------------------------------------
// File: csgvalue.cpp
// 
// Constructive solid geometry evaluation tree value, it's actually the tree leaf
// Value contains scene object to search intersection. 
// This is the final class, implementing IShape interface methods
//
//  
//-------------------------------------------------------------------

#include "csgvalue.h"

CSGValue::CSGValue(IShape* shape)
	: CSGOperand(NULL, NULL),
		mShape(shape)
{
}

CSGValue::~CSGValue()
{
	delete mShape;
	mShape = NULL;
}

CIsect CSGValue::intersect(const Ray& ray)
{
	return mShape->intersect(ray);
}

const Mtrl* CSGValue::getMtrl() const
{
	return mShape->getMtrl();
}

Vec3D CSGValue::getNormal(const Ray& ray, float distance, const CIsect& isect/* = CIsect() */) const
{
	return mShape->getNormal(ray, distance, isect);
}

void CSGValue::setIsLight(bool light)
{
	mShape->setIsLight(light);
}

bool CSGValue::isLight() const
{
	return mShape->isLight();
}

Color CSGValue::getAmbColor(const Vec3D& pnt, const CIsect& isect /*= CIsect()*/) const
{
	return mShape->getAmbColor(pnt, isect);
}


Color CSGValue::getDifColor(const Vec3D& pnt, const CIsect& isect /*= CIsect()*/) const
{
	return mShape->getDifColor(pnt, isect);
}


Color CSGValue::getSpcColor(const Vec3D& pnt, const CIsect& isect /*= CIsect()*/) const
{
	return mShape->getSpcColor(pnt, isect);
}

Vec3D CSGValue::getTexCoords(const Vec3D& pnt, const CIsect& isect /*= CIsect()*/) const
{
	return mShape->getTexCoords(pnt, isect);
}