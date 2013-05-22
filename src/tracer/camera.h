#ifndef TRACER_CAMERA_H
	#define TRACER_CAMERA_H

	#include "geometry/vector3d.h"
	#include "geometry/ray.h"

	// Properties, defining camera orientation and projection
	struct CameraProperties
	{
		Vec3D Eye;
		Vec3D Up;
		Vec3D At;
		float		 Fov;
		float		 NearPlane;
		int			 ImagePlaneW;
		int			 ImagePlaneH;
		bool		 UseExposure;
		bool		 UseGammaCorrection;
	};

	class Camera
	{
	public:
		//! Constructor, setting up camera orientation
		explicit Camera(const CameraProperties& properties);

		//! Update image plane resolution
		void setImagePlaneRes(int width, int height);

		//! Get ray at given image plane coordinates
		Ray lookThrough(int x, int y) const;

		//! Get exposure usage state
		bool hasExposure() const
		{
			return mProperties.UseExposure;
		}

		//! Get gamma correction usage state
		bool hasGammaCorrection() const
		{
			return mProperties.UseGammaCorrection;
		}

	private:
		CameraProperties mProperties;
		// Camera internal orientation and projection data
		Vec3D mXAxis;
		Vec3D mYAxis;
		Vec3D mZAxis;
		float    mAspectRatio;
		float		 mFocus;
	};

#endif // TRACER_CAMERA_H