//-------------------------------------------------------------------
// File: camera.h
// 
// Pin-hole camera implementation
//
//  
//-------------------------------------------------------------------
#define _USE_MATH_DEFINES // For M_PI
#include <math.h>

#include "camera.h"

Camera::Camera(const CameraProperties& properties)
	: mProperties(properties)
{
	mZAxis = (mProperties.At - mProperties.Eye).toUnit();
	mXAxis = cross(mZAxis, mProperties.Up).toUnit();
	mYAxis = cross(mXAxis, mZAxis).toUnit();

	mAspectRatio = static_cast<float>(properties.ImagePlaneW) / properties.ImagePlaneH;

	// Calculate camera focus from fov
	float radAngle = mProperties.Fov * static_cast<float>(M_PI) / 180.f;
	mFocus				 = 1.f / tan(radAngle / 2.f);
}

void Camera::setImagePlaneRes(int width, int height)
{
	mProperties.ImagePlaneW  = width;
	mProperties.ImagePlaneH = height;
	// Also update aspect ratio
	mAspectRatio = static_cast<float>(mProperties.ImagePlaneW) / mProperties.ImagePlaneH;
}

Ray Camera::lookThrough(int x, int y) const
{
	float projectedX	 = (2.f * (static_cast<float>(x) / mProperties.ImagePlaneW - 0.5f) * mAspectRatio);
	float projectedY   = (2.f * (0.5f - static_cast<float>(y) / mProperties.ImagePlaneH));
	Vec3D origin    = mProperties.Eye;
	Vec3D direction = (mXAxis * projectedX + mYAxis * projectedY + mZAxis * mFocus).toUnit();

	return Ray(origin, direction);
}