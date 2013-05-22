//-------------------------------------------------------------------
// File: tracer.h
// 
// Ray tracer algorithm implementation
//
//  
//-------------------------------------------------------------------

#include <assert.h>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <string>

#include "geometry/ray.h"
#include "illumination/lightsource.h"
#include "illumination/material.h"
#include "interfaces/ishape.h"

#include "camera.h"
#include "scene.h"
#include "tracerproperties.h"

#include "tracer.h"

#define EXPOSURE_FACTOR  -1.0f
#define COMPONENTS_COUNT 4
#define RGBA(r, g, b, a) ((a & 0xff) << 24) | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);

#define PRINT_DEBUG
#define USE_SHLICK_APPROXIMATION

namespace 
{
	Vec3D GRefractRay(const Vec3D& sourceDir, 
											 float sourceEnvDensity, 
											 float targetEnvDensity,
											 const Vec3D& outNormal,
											 bool* isTotalInternalReflection)
	{
		const float nue				 = sourceEnvDensity / targetEnvDensity;
		const float cosThetaS  = -dot(outNormal, sourceDir);
		const float cosThetaT2 = 1.f - nue * nue * (1.f - cosThetaS * cosThetaS); // Squared

		if (cosThetaT2 < 0.f)
		{
			*isTotalInternalReflection = true;
			return Vec3D();
		}

		return ((nue * sourceDir) + (nue * cosThetaS - sqrtf(cosThetaT2)) * outNormal).toUnit();
	}

	float GGetFresnelFactor(const Vec3D& sourceDir, 
												  float sourceEnvDensity, 
												  float targetEnvDensity,
												  const Vec3D& outNormal,
												  bool* isTotalInternalReflection)
	{
		const float nue				 = sourceEnvDensity / targetEnvDensity;
		const float cosThetaS  = -dot(outNormal, sourceDir);

		float cosThetaT2 = 1.f - nue * nue * (1.f - cosThetaS * cosThetaS); // Squared

		if (cosThetaT2 < 0.f)
		{
			*isTotalInternalReflection = true;
			return 0.f;
		}

		cosThetaT2 = sqrtf(cosThetaT2);

		// Take into account s- and p-polarized light

		float Rs = (sourceEnvDensity * cosThetaS - targetEnvDensity * cosThetaT2) /
			(sourceEnvDensity * cosThetaS + targetEnvDensity * cosThetaT2);

		Rs *= Rs;

		float Rp = (sourceEnvDensity * cosThetaT2 - targetEnvDensity * cosThetaS) / 
			(sourceEnvDensity * cosThetaT2 + targetEnvDensity * cosThetaS);

		Rp *= Rp;

		return (0.5f * (Rs + Rp));
	}

	float GGetFresnelFactorShlick(const Vec3D& sourceDir, 
																float sourceEnvDensity, 
																float targetEnvDensity,
																const Vec3D& outNormal,
																bool* isTotalInternalReflection)
	{
		float r0;

		r0 = ((sourceEnvDensity - targetEnvDensity) * (sourceEnvDensity - targetEnvDensity)) / 
			((sourceEnvDensity + targetEnvDensity) * (sourceEnvDensity + targetEnvDensity));

		float dirDotNormal = 1 + dot(sourceDir, outNormal);

		return (r0 + (1 - r0) * pow(dirDotNormal, 5));
	}
}


Tracer::Tracer()
	: mCurrentExposureFactor(-1.f)
{
}

Tracer::~Tracer()
{
}

void Tracer::render(const Scene& scene, unsigned char* image)
{
	const int		cImgPlaneW  = scene.getImagePlaneW();
	const int		cImgPlaneH = scene.getImagePlaneH();
	const float cAirRefraction		= Scene::GetDefaultAirProperties()->Refraction;

	//calculateExposure(scene);

	Camera* const camera = scene.getCamera();

	unsigned* data = reinterpret_cast< unsigned* >(image);

	#ifdef PRINT_DEBUG
	const unsigned pixelsCount   = cImgPlaneW * cImgPlaneH;
	float		progress					   = 0;
	double  lastTime					   = 0.0;
	static const char cursor[]   = "-\\|/";
	static int				cursor_idx = 0;
	#endif // PRINT_DEBUG

	for (int y = 0; y < cImgPlaneH; ++y)
	{
		
		for (int x = 0; x < cImgPlaneW; ++x)
		{
			Ray					 ray   = camera->lookThrough(x, y);
			CIsect isect;
			Color				 pixel = compute(scene, 
																	 ray, 
																	 0,								// Initial recursion depth
																	 1.f,						  // Initial reflection intensity
																	 cAirRefraction,  // Ray's starting from air
																	 &isect);

			// Compute exposure for color component, instead of saturation
			float fRed = COLOR_R(pixel);
			float	fGreen = COLOR_G(pixel); 
			float fBlue = COLOR_B(pixel);
			if (camera->hasExposure())
			{
				postprocessColor(pixel, &fRed, &fGreen, &fBlue);
			}
			else
			{
				saturateColor(pixel, &fRed, &fGreen, &fBlue);
			}

			if (camera->hasGammaCorrection())
			{
				fRed	 = gammaCorrection(fRed);
				fGreen = gammaCorrection(fGreen);
				fBlue  = gammaCorrection(fBlue);
			}
			
			// Saturate values
			unsigned char red   = static_cast<unsigned char>(std::min<unsigned>(fRed * 255, 255));
			unsigned char green = static_cast<unsigned char>(std::min<unsigned>(fGreen * 255, 255)); 
			unsigned char blue  = static_cast<unsigned char>(std::min<unsigned>(fBlue * 255, 255));
			
			int index = y * cImgPlaneW + x;
			*(data + index) = RGBA(red, green, blue, 255);

			// Kills perfomance, but gives comfort
			#ifdef PRINT_DEBUG
			double time = static_cast<double>(std::clock());
			if (time - lastTime > CLOCKS_PER_SEC)
			{
				progress = (index * 1.f / pixelsCount * 100.f);
				//GClearConsole();
				
				std::cout << cursor[cursor_idx] << " ";	
				cursor_idx = (cursor_idx + 1) % (sizeof(cursor) - 1);

				std::cout << "Progress: " << progress << "%; ";
				std::cout << "Passed pixel " << x << ", " << y << "\r";
				lastTime = time;
			}
			#endif // PRINT_DEBUG
		}
	}

	std::cout << "Progress: 100%; Rendering finished!" << std::endl;
}

Color Tracer::compute(const Scene& scene, 
											const Ray& ray, 
											int recursionDepth, 
											float reflectionIntensity,
											float sourceEnvDensity,
											CIsect *out)
{
	const int													cMaxRecursionDepth = scene.getTracerProperties()->MaxRayRecursionDepth;

	if (cMaxRecursionDepth >= 0 && recursionDepth > cMaxRecursionDepth)
	{
		return Color();
	}
	
	bool	reflected = recursionDepth != 0;
	Color resultColor;	
	
	CIsect intersection = scene.intersect(ray, false);
	*out = intersection;
	if (!intersection.Exists) // No intersections found
	{
		if (reflected)
		{
			return resultColor;
		}
		return getBackgroundColor(scene);
	}

	IShape *object = intersection.Object; 
	if (object->isLight())
	{
		if (reflected)
		{
			return resultColor;
		}

		const Mtrl* material = object->getMtrl();
		return material->AmbColor + material->DifColor + material->SpcColor;
	}

  const Vec3D  isectPoint		 = ray.apply(intersection.Distance);
  const Mtrl *objectMtrl = object->getMtrl();

	// Normal of the intersected object
	const Vec3D normal = intersection.Normal;
		
	// The more rays are computed, the less intensivity will be
	resultColor += (scene.illuminate(ray, object, intersection.Distance, normal));

	const Vec3D& rayDir = ray.getDir();
	const float viewProjection   = dot(rayDir, normal);
	// Turn normal to the front side anyway
	Vec3D outNormal = normal;
	if (viewProjection > 0.f)
	{
		outNormal = -normal;
	}
	// Calculate fresnel factor
	bool isTotalInternalReflection = false;
	float fresnel = GGetFresnelFactor(rayDir, sourceEnvDensity, objectMtrl->Density, outNormal, &isTotalInternalReflection);

	// Calculate reflection
	if (objectMtrl->Reflection > 0.f && reflectionIntensity > EPSILON)
	{
		// Reflect ray	
		const Vec3D& rayDir = ray.getDir();
		Vec3D direction = rayDir - 2.f * dot(rayDir, outNormal) * outNormal;
		CIsect reflected;
		Color reflectedColor = (compute(scene, 
																	  Ray(isectPoint + direction * EPSILON, direction), 
																		recursionDepth + 1, 
																		reflectionIntensity * objectMtrl->Reflection, 
																		objectMtrl->Density,
																		&reflected) * reflectionIntensity * objectMtrl->Reflection * fresnel);
		 resultColor += scale3D(reflectedColor, objectMtrl->DifColor);
	}
	// Calculate refraction
	if (objectMtrl->Refraction > 0.f)
	{
		const float viewProjection = dot(rayDir, normal);
		const float density				 = objectMtrl->Density;

		const float nue						 = sourceEnvDensity / density;

		const Vec3D direction = GRefractRay(rayDir, sourceEnvDensity, density, outNormal, &isTotalInternalReflection);

		if (!isTotalInternalReflection)
		{
			CIsect	 refractedIsect;
			Color refracted  = compute(scene, 
																 Ray(isectPoint + direction * EPSILON, direction), 
																 recursionDepth + 1, 
																 reflectionIntensity, 
																 density, 
																 &refractedIsect);
			// Apply Beer's law
			if (refractedIsect.Exists)
			{
				Color absorbance   = (objectMtrl->DifColor) * 0.15f * (-refractedIsect.Distance);
				Color transparency = Color(expf(COLOR_R(absorbance)), expf(COLOR_G(absorbance)), expf(COLOR_B(absorbance)));
				resultColor += scale3D(refracted,  transparency) * (1.f - fresnel);
			}
		}
	}
			
	return resultColor;
}

void Tracer::postprocessColor(const Color& color, float *r, float *g, float *b)
{
	*r = 1.f - expf(COLOR_R(color) * mCurrentExposureFactor);
	*g = 1.f - expf(COLOR_G(color) * mCurrentExposureFactor);
	*b = 1.f - expf(COLOR_B(color) * mCurrentExposureFactor);
}

void Tracer::saturateColor(const Color& color, float *r, float *g, float *b)
{
	*r = std::min(COLOR_R(color), 1.f);
	*g = std::min(COLOR_G(color), 1.f);
	*b = std::min(COLOR_B(color), 1.f);
}

float Tracer::gammaCorrection(float color)
{
	// Use sRGB encoding
	if (color < 0.0031308f)
	{
		return 12.92f * color;
	}
	else
	{
		return 1.055f * powf(color, 0.4166667f) - 0.055f; // Inverse gamma 2.4
	}
}

Color Tracer::getBackgroundColor(const Scene& scene)
{
	// Return ambient part of background color for now or forever
	return scene.getBackground()->AmbColor;
}

Ray Tracer::reflectRay(const Vec3D& reflectedOrg, const Vec3D& source, const Vec3D& over)
{
	return Ray(reflectedOrg, (source - 2 * dot(source, over) * over).toUnit());
}

void Tracer::calculateExposure(const Scene& scene)
{
	static const int   cAccumulationSize			 = 16;
	static const float cMediumPointWeight			 = 1.f / (cAccumulationSize * cAccumulationSize);
	static const float cDefaultTonemapMidpnt = 0.7f;

	// Algorithm taken somewhere across the internet
	const int		cImgPlaneW  = scene.getImagePlaneW();
	const int		cImgPlaneH = scene.getImagePlaneH();

	float exposure			 = -1.f;
	float accuracyFactor = static_cast<float>(std::max(cImgPlaneW, cImgPlaneH));

	accuracyFactor /= cAccumulationSize;

	float mediumPoint = 0.f;

	const Camera* camera = scene.getCamera();
	for (int y = 0; y < cAccumulationSize; ++y)
	{
		for (int x = 0; x < cAccumulationSize; ++x)
		{
			Ray					 viewRay = camera->lookThrough(x * accuracyFactor, y * accuracyFactor);
			CIsect isect;
			Color				 result = compute(scene, viewRay, 0, 1.f, 1.f, &isect);

			float	luminance = 0.2126f   * COLOR_R(result) + 
												0.71516f  * COLOR_G(result) +
												0.072169f * COLOR_B(result);
			mediumPoint += cMediumPointWeight * (luminance * luminance);
		}
	}

	float mediumLuminance = sqrtf(mediumPoint);

	if (mediumLuminance > 0.f)
	{
		exposure = logf(1.f - cDefaultTonemapMidpnt) / mediumLuminance;
	}

	mCurrentExposureFactor = exposure;
}
