//-------------------------------------------------------------------
// File: lightsource.cpp
// 
// Light source properties collection
//
//  
//-------------------------------------------------------------------

#include "geometry/vector3d.h"
#include "geometry/ray.h"
#include "interfaces/ishape.h"
#include "tracer/scene.h"
#include "material.h"

#include "lightsource.h"

Color PointLightSource::computeColor(const Scene& scene, IShape* object, const Ray& viewRay, float distance, const Vec3D& normal) const
{
	const Mtrl *objectMtrl  = object->getMtrl();
	const Vec3D  objSurfacePoint = viewRay.apply(distance);
	Color ambientTerm = scale3D(object->getAmbColor(objSurfacePoint), AmbIntensity);
	Color diffuseTerm;
	Color specularTerm;
	Color result			= ambientTerm;

	Vec3D shadowRayDir = Position - objSurfacePoint;
	const float		 distanceToLight	  = length(shadowRayDir);
	const float		 attenuation				= 1 / (ConstantAttenutaion + LinearAttenutaion * distanceToLight + QuadraticAttenutaion * distanceToLight * distanceToLight);
	// Avoid twice length calculation, similar to normalize
	shadowRayDir /= distanceToLight; 
	//result *= attenuation;
	const float	cosShadowNormal	= dot(shadowRayDir, normal);
  result *= attenuation;
	// Light is on the other side, we're illuminating front one
	if (cosShadowNormal <= 0.f)
	{
		return object->getAmbColor(objSurfacePoint);
	}

	const Ray shadowRay(objSurfacePoint + shadowRayDir * EPSILON, shadowRayDir);	

	const CIsect lightCIsect = scene.intersect(shadowRay, true);
	// Object not in the shadow
	if (!lightCIsect.Exists || lightCIsect.Object->isLight() || lightCIsect.Distance > distanceToLight)
	{
		const Color diffuseColor = object->getDifColor(objSurfacePoint);
		diffuseTerm  = scale3D(diffuseColor, cosShadowNormal * DifIntensity * attenuation);

		const Vec3D lightReflect = (shadowRayDir - 2 * dot(shadowRayDir, normal) * normal).toUnit();

		const Vec3D cameraDir = (viewRay.getOrg() - objSurfacePoint).toUnit();
		//float	cosLightReflect = dot(shadowRayDir, lightReflect);
		const float	cosLightReflect = dot(cameraDir, lightReflect);

		if (cosLightReflect > 0.0f)
		{
			const Color specularColor = object->getSpcColor(objSurfacePoint); 

			specularTerm	= scale3D(specularColor, SpcIntensity * powf(cosLightReflect, objectMtrl->SpcPower) * attenuation);
		}				
	}
	// Compute color, see Phong model
	result += (diffuseTerm + specularTerm);
	return result;
}

Color DiralLightSource::computeColor(const Scene& scene, IShape* object, const Ray& viewRay, float distance, const Vec3D& normal) const
{
	const Mtrl *objectMtrl  = object->getMtrl();
	const Vec3D  objSurfacePoint = viewRay.apply(distance);
	Color ambientTerm = scale3D(object->getAmbColor(objSurfacePoint), AmbIntensity);
	Color diffuseTerm;
	Color specularTerm;
	Color result			= ambientTerm;

	const Vec3D lightVector	= -Dir;
	
  const Vec3D surfaceLightVector = (Position - objSurfacePoint);
  const float lightDistance				  = dot(surfaceLightVector, lightVector);

	// Return only object's color, if it's too far away from the light source
  if (lightDistance > LightRange)
	{
		return object->getAmbColor(objSurfacePoint);
	}
	
  const float	cosLightNormal	= dot(lightVector, normal);
	// Light is on the other side, we're illuminating front one
	if (cosLightNormal <= 0.f)
	{
		return result;
	}

	const Ray shadowRay(objSurfacePoint + lightVector * EPSILON, lightVector);	

	const CIsect lightCIsect = scene.intersect(shadowRay, true);
	// Object not in the shadow
	if (!lightCIsect.Exists || lightCIsect.Object->isLight() || lightCIsect.Distance > lightDistance)
	{
		const Color diffuseColor = object->getDifColor(objSurfacePoint);
		diffuseTerm  = scale3D(diffuseColor, cosLightNormal * DifIntensity);

		const Vec3D lightReflect = (Dir - 2 * dot(Dir, normal) * normal).toUnit();

		const Vec3D cameraDir = (viewRay.getOrg() - objSurfacePoint).toUnit();
		//float	cosLightReflect = dot(shadowRayDir, lightReflect);
		const float	cosLightReflect = dot(cameraDir, lightReflect);

		if (cosLightReflect > 0.0f)
		{
			const Color specularColor = object->getSpcColor(objSurfacePoint); 

			specularTerm	= scale3D(specularColor, SpcIntensity * powf(cosLightReflect, objectMtrl->SpcPower));
		}				
	}
	// Compute color, see Phong model
	result += (diffuseTerm + specularTerm);
	return result;
}

Color SpotLightSource::computeColor(const Scene& scene, IShape* object, const Ray& viewRay, float distance, const Vec3D& normal) const
{
	const Mtrl *objectMtrl  = object->getMtrl();
	const Vec3D  objSurfacePoint = viewRay.apply(distance);
	Color ambientTerm = scale3D(object->getAmbColor(objSurfacePoint), AmbIntensity);
	Color diffuseTerm;
	Color specularTerm;
	Color result			= ambientTerm;

	Vec3D lightVector	= -Dir;
	const float	cosLightNormal	= dot(lightVector, normal);

  const Vec3D lightDir = (Position - objSurfacePoint).toUnit();

	// Light is on the other side, we're illuminating front one
	if (cosLightNormal <= 0.f)
	{
		return object->getAmbColor(objSurfacePoint);
	}
	
	const float distanceToLight			= length(Position - objSurfacePoint);
	const float distanceAttenuation = 1 / (ConstantAttenutaion + LinearAttenutaion * distanceToLight + QuadraticAttenutaion * distanceToLight * distanceToLight);
	
  float cosCurrentAngle     = dot(lightDir, lightVector);
  if (cosCurrentAngle <= 0.f)
  {
    return objectMtrl->AmbColor;
  }

	result *= distanceAttenuation;

	const float rho = dot(lightDir, lightVector);
  float spotAttenuation = 0.f;
	if (rho > CosHalfUmbraAngle)
	{
		spotAttenuation = 1.f;
	}
	else if (rho < CosHalfPenumbraAngle)
	{
		spotAttenuation = 0.f;
	}
	else
	{
		const float factor = (rho - CosHalfPenumbraAngle) / (CosHalfUmbraAngle - CosHalfPenumbraAngle);
		spotAttenuation = powf(factor, SpotlightFalloff);
	}

  result *= spotAttenuation;

	const Ray shadowRay(objSurfacePoint + lightVector * EPSILON, lightVector);	

	const CIsect lightCIsect = scene.intersect(shadowRay, true);
	// Object not in the shadow
	if (!lightCIsect.Exists || lightCIsect.Object->isLight() || lightCIsect.Distance > distanceToLight)
	{
		//result *= spotAttenuation;
		const Color diffuseColor = object->getDifColor(objSurfacePoint);
		diffuseTerm  = scale3D(diffuseColor, cosLightNormal * DifIntensity * spotAttenuation * distanceAttenuation);

		const Vec3D lightReflect = (Dir - 2 * dot(Dir, normal) * normal).toUnit();

		const Vec3D cameraDir = (viewRay.getOrg() - objSurfacePoint).toUnit();
		//float	cosLightReflect = dot(shadowRayDir, lightReflect);
		float	cosLightReflect = dot(cameraDir, lightReflect);

		if (cosLightReflect > 0.0f)
		{
			const Color specularColor = object->getSpcColor(objSurfacePoint); 

			specularTerm	= scale3D(specularColor, SpcIntensity * powf(cosLightReflect, objectMtrl->SpcPower) * spotAttenuation * distanceAttenuation);
		}				
	}
	// Compute color, see Phong model
	result += (diffuseTerm + specularTerm);
	return result;
}
