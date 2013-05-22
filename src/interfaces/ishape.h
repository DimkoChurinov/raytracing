#ifndef INTERFACES_ISHAPE_H
	#define INTERFACES_ISHAPE_H

	#include "geometry/ray.h"
	#include "geometry/intersection.h"
	#include "illumination/types.h"

	class Mtrl;


	struct IShape
	{
		virtual ~IShape() = 0
		{
		}

		//! Find intersection with the ray and return data
		virtual CIsect intersect(const Ray& ray) = 0;

		//! Get material of the shape
		virtual const Mtrl* getMtrl() const = 0;

		//! Get normal of the shape at given distance and ray with additional data about intersection
		virtual Vec3D getNormal(const Ray& ray, float distance, const CIsect& isect = CIsect()) const = 0;

		//! Set that object is light source
		virtual void setIsLight(bool light) = 0;

		//! Get object's light state
		virtual bool isLight() const = 0;

		//! Get color at given pnt, returns either the texel of an ambient texture or ambient color of material in given pnt
		virtual Color getAmbColor(const Vec3D& pnt, const CIsect& isect = CIsect()) const = 0;

		//! Get color at given pnt, returns either the texel of an diffuse texture or diffuse color of material in given pnt
		virtual Color getDifColor(const Vec3D& pnt, const CIsect& isect = CIsect()) const = 0;

		//! Get color at given pnt, returns either the texel of an specular texture or specular color of material in given pnt
		virtual Color getSpcColor(const Vec3D& pnt, const CIsect& isect = CIsect()) const = 0;

		//! Get texture coordinates at given pnt, 2 first components of vector will be used, and one will be ommited
		virtual Vec3D getTexCoords(const Vec3D& pnt, const CIsect& isect = CIsect()) const = 0;

		// TODO: Add BBox etc.
	};


#endif // INTERFACES_ISHAPE_H