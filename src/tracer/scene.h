#ifndef TRACER_SCENE_H
	#define TRACER_SCENE_H
	
	#include <vector>

	#include "geometry/intersection.h"

	#include "illumination/types.h"

	class	 Camera;
	struct CameraProperties;
	struct IShape;
	struct LightSource;
	struct Mtrl;
	class  Ray;
	struct TracerProperties;
	

	class Scene
	{
	public:
		//! Get default material properties for air
		static const Mtrl* GetDefaultAirProperties();

	public:
		explicit Scene();

		~Scene();

		//! Find closest intersection with one of the scene objects
		CIsect intersect(const Ray& ray, bool stopIfFound) const;

		//! Illuminate scene in given pnt (calculated along ray direction at given distance)
		Color illuminate(const Ray& viewRay, IShape* object, float distance, const Vec3D& normal) const;

		void addObject(IShape* object);

		void addLightSource(LightSource* light);

		void setBackground(Mtrl* bgMtrl);

		void setupCamera(const CameraProperties& properties);

		void setTracerProperties(TracerProperties* properties);

		void setImagePlaneRes(int resX, int resY);

		void clear();

		void setTracerDepth(float depth);

		const std::vector< IShape* > &getObjects() const
		{
			return mObjects;
		}

		const std::vector< LightSource* > &getLights() const
		{
			return mLights;
		}

		Mtrl* const getBackground() const
		{
			return mBackground;
		}

		Camera* const getCamera() const
		{
			return mCamera;
		}

		TracerProperties* const getTracerProperties() const
		{
			return mTracerProperties;
		}

		int getImagePlaneW() const
		{
			return mImagePlaneW;
		}

		int getImagePlaneH() const
		{
			return mImagePlaneH;
		}

	private:
		// Disable copy and assignment
		Scene(const Scene&);
		Scene& operator=(const Scene&);

	private:
		std::vector< IShape* >			mObjects;
		std::vector< LightSource* > mLights;
		Mtrl									 *mBackground;
		Camera										 *mCamera;
		TracerProperties					 *mTracerProperties;
		float												mTracerDepth;
		// Explicitly store dimensions of the scene
		int													mImagePlaneW;
		int													mImagePlaneH;
	};

#endif // TRACER_SCENE_H