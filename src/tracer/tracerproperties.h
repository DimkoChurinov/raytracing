#ifndef TRACER_TRACERPROPERTIES_H
	#define TRACER_TRACERPROPERTIES_H

	struct TracerProperties
	{
		int MaxRayRecursionDepth;  // Max number of recursive ray applications
		int MaxRayReflectionDepth; // Max number of reflection rays
	};

#endif // TRACER_TRACERPROPERTIES_H