//-------------------------------------------------------------------
// File: model.cpp
// 
// Model scene object implementation
//
//  
//-------------------------------------------------------------------

#include "illumination/material.h"
#include "triangle.h"

#include "model.h"

#define TOO_FAR_AWAY		 1000000.f

Model::Model(const std::vector< ModelTriangle* > &triangles, const BBox &bbox, Mtrl* material)
	: mTriangles(triangles),
		mBoundingBox(bbox),
		mMtrl(material),
		mIsLight(false)
{
	
}

Model::~Model()
{
	for (std::vector< ModelTriangle* >::iterator tri = mTriangles.begin(); tri != mTriangles.end(); ++tri)
	{
		delete *tri;
		*tri = NULL;
	}
	delete mMtrl;
}

CIsect Model::intersect(const Ray& ray)
{
	if (!mBoundingBox.intersect(ray))
	{
		return CIsect(false);
	}

	float				 closestDistance = TOO_FAR_AWAY;
	CIsect closestCIsect(false);
	for (std::vector< ModelTriangle* >::iterator tri = mTriangles.begin(); tri != mTriangles.end(); ++tri)
	{
		CIsect current = (*tri)->intersect(ray);
		if (current.Exists)
		{
			closestCIsect.Dst.push_back(current.Distance);
			if (current.Distance < closestDistance)
			{
				closestCIsect.Exists	 = true;
				closestCIsect.Distance = current.Distance;
				closestCIsect.Object	 = this;
				closestCIsect.Normal	 = current.Normal;

				closestDistance = current.Distance;
			}
		}
	}

	return closestCIsect;
}


Vec3D Model::getNormal(const Ray& ray, float distance, const CIsect& isect /*= CIsect()*/) const
{
	return isect.Normal; // Return already calculated normal
}

Color Model::getAmbColor(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	return mMtrl->AmbColor;
}

Color Model::getDifColor(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	if (mMtrl->DifTexture)
	{
		Vec3D texCoords = getTexCoords(pnt, isect);
		return scale3D(mMtrl->DifTexture->sample(texCoords.x(), texCoords.y()), mMtrl->DifColor);
	}
	return mMtrl->DifColor;
}

Color Model::getSpcColor(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	return mMtrl->SpcColor;
}

Vec3D Model::getTexCoords(const Vec3D& pnt, const CIsect& isect/* = CIsect()*/) const
{
	return isect.TexCoords;
}