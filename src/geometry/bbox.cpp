//-------------------------------------------------------------------
// File: bbox.h
// 
// Scene object's axis-aligned bounding box implementation implementation
//
//  
//-------------------------------------------------------------------

#include <cfloat>
#include <utility>
#include <xutility>

#include "precision.h"
#include "ray.h"

#include "bbox.h"

#define Infinity FLT_MAX

bool BBox::intersect(const Ray& ray) const
{
	const Vec3D& origin    = ray.getOrg();
	const Vec3D& direction = ray.getDir();

	float d0 = -Infinity, d1 = Infinity;

  if(fabs(direction.x()) > FLOAT_ZERO)
  {
    d0 = (Min.x() - origin.x()) / direction.x();
    d1 = (Max.x() - origin.x()) / direction.x();
    if(d1 < d0)
		{
        std::swap(d0, d1);
    }
	}
 
    if(fabs(direction.y()) > FLOAT_ZERO)
    {
        float t0, t1;
        t0 = (Min.y() - origin.y()) / direction.y();
        t1 = (Max.y() - origin.y()) / direction.y();
        if(t1 < t0)
				{
            std::swap(t0, t1);
				}
        d0 = std::max(d0,t0);
        d1 = std::min(d1,t1);
    }
 
    if(fabs(direction.z()) > FLOAT_ZERO)
    {
        float t0, t1;
        t0 = (Min.z() - origin.z()) / direction.z();
        t1 = (Max.z() - origin.z()) / direction.z();
        if(t1 < t0)
				{
            std::swap(t0, t1);
				}
        d0 = std::max(d0,t0);
        d1 = std::min(d1,t1);
    }
 
  if(d1 < d0 || d0 == -Infinity)
	{
      return false;
	}
  else
	{
      return true;
	}
}