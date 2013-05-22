#ifndef TRACER_TRACER_H
	#define TRACER_TRACER_H

	#include <vector>

	#include "geometry/intersection.h"
	#include "illumination/types.h"

	struct IShape;
	class Scene;
	class Ray;
	
	class Tracer
	{
	public:
		Tracer();

		~Tracer();

		//! Render given scene to the image data array of size width * height * 4 with format ARGB32
		void render(const Scene& scene, unsigned char* image);


	private:
		//! Find ray intersection with given scene at given coordinates and return computed color
		Color compute(const Scene& scene, 
									const Ray& ray, 
									int recursionDepth, 
									float reflectionIntensity, 
									float sourceEnvDensity, 
									CIsect* out);

		//! Apply postprocessing to computed color
		void postprocessColor(const Color& color, float *r, float *g, float *b);

		//! Saturate given color
		void saturateColor(const Color& color, float *r, float *g, float *b);

		//! Apply gamma correction for color component
		float gammaCorrection(float color);

		//! Get scene background color
		Color getBackgroundColor(const Scene& scene);

		//! Reflect source ray over given vector
		Ray reflectRay(const Vec3D& reflectedOrg, const Vec3D& source, const Vec3D& over);

		//! Calculate exposure factor for the given scene
		void calculateExposure(const Scene& scene);

	private:
		//! Current scene exposure factor
		float mCurrentExposureFactor;
	};

#endif // TRACER_TRACER_H
