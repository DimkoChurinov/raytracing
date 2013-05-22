#ifndef ILLUMINATION_LIGHTSOURCE_H
#define ILLUMINATION_LIGHTSOURCE_H

#include "types.h"

struct IShape;
class  Ray;
class  Scene;
	
enum LightSourceType
{
	LIGHTSOURCE_POINT,
	LIGHTSOURCE_DIRECTIONAL,
	LIGHTSOURCE_SPOT
};

struct LightSource
{
	Vec3D	Position;
	Vec3D	Dir;						// Its direction
	Color	AmbIntensity;			// Light source ambient color intensity
	Color	DifIntensity;		  // Light source diffuse color intensity
	Color	SpcIntensity;	  // Light source specular color intensity
	float	ConstantAttenutaion;	// Constant attenuation coefficient
	float					  LinearAttenutaion;		// Constant attenuation coefficient
	float					  QuadraticAttenutaion;	// Constant attenuation coefficient
	float						LightRange;						// Light range for directional light
	float						PenumbraAngle;				// Penumbra angle of spotlight in radians (UmbraAngle, Pi)
	float						UmbraAngle;						// Umbra angle of spotligh in radians (0, Pi)
	float						SpotlightFalloff;			// Spotlight fallof factor
	LightSourceType Type;									// Lighting type
	// Precalculated values
	float						CosHalfUmbraAngle;		// Inplace calculate values, that will be used in computations, this is cosf(UmbraAngle / 2.f)
	float						CosHalfPenumbraAngle; // Inplace calculate values, that will be used in computations, this is cosf(PenumbraAngle / 2.f)

	virtual Color computeColor(const Scene& scene, IShape* object, const Ray& viewRay, float distance, const Vec3D& normal) const = 0;
};

struct PointLightSource : LightSource
{
	virtual Color computeColor(const Scene& scene, IShape* object, const Ray& viewRay, float distance, const Vec3D& normal) const;
};

struct DiralLightSource : LightSource
{
	virtual Color computeColor(const Scene& scene, IShape* object, const Ray& viewRay, float distance, const Vec3D& normal) const;
};

struct SpotLightSource : LightSource
{
	virtual Color computeColor(const Scene& scene, IShape* object, const Ray& viewRay, float distance, const Vec3D& normal) const;
};

#endif