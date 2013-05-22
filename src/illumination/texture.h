#ifndef ILLUMINATION_TEXTURE_H
	#define ILLUMINATION_TEXTURE_H

	#include "types.h"

	class Texture
	{
	public:
		explicit Texture(Color* data = 0x0, int width = 0, int height = 0);

		virtual ~Texture();

		//! Get texel at given coordinates
		virtual Color sample(float u, float v) const;

		int getWidth() const
		{
			return mWidth;
		}

		int getHeight() const
		{
			return mHeight;
		}

	private:
		Color sampleNearestRepeat(float u, float v) const;

	private:
		//! Texture color data, texture manages these data and deletes this upon destruction
		Color* mData;
		//! Texture dimensions
		int		 mWidth;
		int		 mHeight;
	};

#endif // ILLUMINATION_TEXTURE_H