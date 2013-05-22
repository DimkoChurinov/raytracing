#ifndef ILLUMINATION_TYPES_H
	#define ILLUMINATION_TYPES_H

	#include "geometry/vector3d.h"

	//! Inherit all operations from vector
	typedef Vec3D Color;

	// Components access with more common names, rather than x, y, z
	#define COLOR_R(v) (v).x()
	#define COLOR_G(v) (v).y()
	#define COLOR_B(v) (v).z()
	#define COLOR_SET_R(v, r) (v).setX((r))
	#define COLOR_SET_G(v, g) (v).setY((g))
	#define COLOR_SET_B(v, b) (v).setZ((b))


#endif // ILLUMINATION_TYPES_H