//-------------------------------------------------------------------
// File: scene.cpp
// 
// Scene properties collection
//
//  
//-------------------------------------------------------------------

#include "illumination/lightsource.h"
#include "illumination/material.h"
#include "interfaces/ishape.h"
#include "camera.h"
#include "tracerproperties.h"

#include "scene.h"

#define TOO_FAR_AWAY		 1000000.f

const Mtrl* Scene::GetDefaultAirProperties()
{
	static Mtrl cAir;
	
	cAir.Refraction = 1.f;			// No refraction
	cAir.Reflection = 0.f;			// No reflection
	cAir.Density    =	1.f;			// = No refraction
	
	return &cAir;
}

Scene::Scene()
	: mBackground(NULL),
		mCamera(NULL),
		mTracerProperties(NULL),
		mTracerDepth(0.f)
{
}

Scene::~Scene()
{
	clear();
}

CIsect Scene::intersect(const Ray& ray, bool stopIfFound) const
{
	// Find closest ray object intersection
	float				 closestDistance = TOO_FAR_AWAY;
	bool				 anyFound				 = false;
	CIsect closestCIsect(false);

	for (int obj = 0, count = mObjects.size(); obj < count; ++obj)
	{
		CIsect isect = mObjects[obj]->intersect(ray);
		if (!isect.Exists)
		{
			continue;
		}

		// Filter depth, if it's set, I mean if it's greater than zero
		if (mTracerDepth > 0.f && isect.Distance > mTracerDepth)
		{
			continue;
		}

		if (isect.Distance < closestDistance || !anyFound)
		{
			closestDistance			= isect.Distance;
			closestCIsect = isect;
			anyFound						= true;
		}

		if (stopIfFound)
		{
			return closestCIsect;
		}
	}

	return closestCIsect;
}

Color Scene::illuminate(const Ray& viewRay, IShape* object, float distance, const Vec3D& normal) const
{
	Color resultColor;
	for (int light = 0, count = mLights.size(); light < count; ++light)
	{
		const LightSource* source = mLights[light];

		// The more rays are computed, the less intensivity will be
		resultColor += (source->computeColor(*this, object, viewRay, distance, normal));
	}
	return resultColor;
}

void Scene::addObject(IShape *object)
{
	mObjects.push_back(object);
}

void Scene::addLightSource(LightSource *light)
{
	mLights.push_back(light);
}

void Scene::setBackground(Mtrl* bgMtrl)
{
	if (mBackground)
	{
		delete mBackground;
		mBackground = NULL;
	}
	mBackground = bgMtrl;
}

void Scene::setupCamera(const CameraProperties& properties)
{
	if (mCamera)
	{
		delete mCamera;
		mCamera = NULL;
	}
	mCamera = new Camera(properties);
}

void Scene::setTracerProperties(TracerProperties* properties)
{
	if (mTracerProperties)
	{
		delete mTracerProperties;
		mTracerProperties = NULL;
	}
	mTracerProperties = properties;
}

void Scene::setImagePlaneRes(int resX, int resY)
{
	mImagePlaneW  = resX;
	mImagePlaneH = resY;
	if (mCamera)
	{
		mCamera->setImagePlaneRes(resX, resY);
	}
}

void Scene::clear()
{
	for (int idx = 0, count = mObjects.size(); idx < count; ++idx)
	{
		delete mObjects[idx];
		mObjects[idx] = NULL;
	}
	mObjects.clear();
	for (int idx = 0, count = mLights.size(); idx < count; ++idx)
	{
		delete mLights[idx];
		mLights[idx] = NULL;
	}
	mLights.clear();

	delete mBackground;
	mBackground = NULL;
	delete mCamera;
	mCamera = NULL;
	delete mTracerProperties;
	mTracerProperties = NULL;
}

void Scene::setTracerDepth(float depth)
{
	mTracerDepth = depth;
}