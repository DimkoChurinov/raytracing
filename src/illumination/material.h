#ifndef ILLUMINATION_MATERIAL_H
	#define ILLUMINATION_MATERIAL_H

	#include "texture.h"
	#include "types.h"
	
	struct Mtrl
	{
		Mtrl()
			: DifTexture(0x0)
		{
		}

		~Mtrl()
		{
			if (DifTexture)
			{
				delete DifTexture;
				DifTexture  = 0x0;
			}
		}

		// General color properties, according to Phong color model
		Color						EmissiveColor;
		Color						AmbColor;	 
		Color						DifColor;
		Color						SpcColor;
		float						SpcPower;
		// or
		Texture				 *DifTexture;
		// Mtrl properties
		float						Refraction;
		float						Reflection;
		float						Density;
		float						Illumination;
		// Texture properties
		float						TexScaleU;
		float						TexScaleV;
	};

#endif // ILLUMINATION_MATERIAL_H