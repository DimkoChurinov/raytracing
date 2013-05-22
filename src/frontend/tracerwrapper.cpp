//-------------------------------------------------------------------
// File: tracerwrapper.cpp
// 
// Qt based ray tracer wrapper
//
//  
//-------------------------------------------------------------------

#include "tracer/scene.h"
#include "tracer/tracer.h"
#include "tracer/tracerproperties.h"

#include "sceneserializable.h"

#include "tracerwrapper.h"

TracerWrapper::TracerWrapper()
	: mTracerDepth(0)
{
}

TracerWrapper::~TracerWrapper()
{
}

bool TracerWrapper::loadScene(const QString& fileName)
{
	SceneSerializable reader;

	mScene = reader.readScene(fileName);

	return !!mScene;
}

void TracerWrapper::renderScene(int resolutionX, int resolutionY, int width, int height)
{
	mScene->setImagePlaneRes(resolutionX, resolutionY);
	TracerProperties* props = mScene->getTracerProperties();

	props->MaxRayRecursionDepth = mTracerDepth;

	Tracer rayTracer;

	mTracerOutput = QImage(resolutionX, resolutionY, QImage::Format_ARGB32);

	rayTracer.render(*mScene, mTracerOutput.bits());

	mRenderImage = mTracerOutput.scaled(width, height, Qt::KeepAspectRatio);
}

void TracerWrapper::saveSceneImage(const QString& fileName)
{
	mTracerOutput.save(fileName);
}

void TracerWrapper::setRecursionDepth(int depth)
{
	mTracerDepth = depth;
}