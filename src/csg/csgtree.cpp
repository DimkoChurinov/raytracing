//-------------------------------------------------------------------
// File: csgtree.cpp
// 
// Constructive solid geometry evaluation tree 
// CSG tree is a binary tree starting in given root, typically an operation
// Tree extends IShape interface with tree-related methods
// There's no need to write some generic algorithms on the tree, because it will
// be constructed from XML, so tree supports only operation of setting root node.
//
//  
//-------------------------------------------------------------------

#include "csgtree.h"

CSGTree::CSGTree(CSGNode* root)
	: mRoot(root)
{

}

CSGTree::~CSGTree()
{
	if (mRoot)
	{
		mRoot->rmvChild();
		delete mRoot;
	}
}

CIsect CSGTree::intersect(const Ray& ray) 
{
	// Simply traverse tree, starting from the root
	return mRoot->intersect(ray);
}

const Mtrl* CSGTree::getMtrl() const
{
	return NULL;
}

Vec3D CSGTree::getNormal(const Ray& ray, float distance, const CIsect& isect/* = CIsect() */) const
{
	// Falback to the intersection cached data
	return isect.Normal;
}

void CSGTree::setIsLight(bool)
{
	/** Can't be light **/
}

bool CSGTree::isLight() const
{
	/** Can't be light **/
	return false;
}

Color CSGTree::getAmbColor(const Vec3D& pnt, const CIsect& isect /*= CIsect()*/) const
{
	// Fallback to data, cached in intersection
	return isect.Object->getAmbColor(pnt, isect);
}


Color CSGTree::getDifColor(const Vec3D& pnt, const CIsect& isect /*= CIsect()*/) const
{
	// Fallback to data, cached in intersection
	return isect.Object->getDifColor(pnt, isect);
}


Color CSGTree::getSpcColor(const Vec3D& pnt, const CIsect& isect /*= CIsect()*/) const
{
	// Fallback to data, cached in intersection
	return isect.Object->getSpcColor(pnt, isect);
}

Vec3D CSGTree::getTexCoords(const Vec3D& pnt, const CIsect& isect /*= CIsect()*/) const
{
	// Fallback to data, cached in intersection
	return isect.Object->getTexCoords(pnt);
}
