//-------------------------------------------------------------------
// File: texture.cpp
// 
// Texture, that can be applied for an object
//
//  
//-------------------------------------------------------------------

#include <cassert>
#include <utility>

#include "texture.h"

Texture::Texture(Color* data, int width, int height)
	: mData(data),
		mWidth(width),
		mHeight(height)
{
}

Texture::~Texture()
{
	if (mData)
	{
		delete[] mData;
		mData = 0x0;
	}
}

Color Texture::sample(float u, float v) const
{
	typedef std::pair<float, float> Pair;
	//
	// Fetch a bilinearly filtered texel
	//

	// Add 0.5, since the texel is sampled in the middle of the pixel 
	const float px = mWidth  * u + 0.5f;
	const float py = mHeight * v + 0.5f;

	// Get sampling positions of neighbour texels
	const Pair uv[2][2] = {
		{ std::make_pair(floorf(px - 1),  floorf(py - 1)), std::make_pair(floorf(px - 1), floorf(py)), },
		{ std::make_pair(floorf(px),		  floorf(py - 1)), std::make_pair(floorf(px),		  floorf(py))  },
	};

	const Color tex[2][2] = {
		{ sampleNearestRepeat(uv[0][0].first, uv[0][0].second), sampleNearestRepeat(uv[0][1].first, uv[0][1].second), },
		{ sampleNearestRepeat(uv[1][0].first, uv[1][0].second), sampleNearestRepeat(uv[1][1].first, uv[1][1].second), },
	};

	// Get fractional parts of coordinates to interpolate values
	const float fx = px - floorf(px);
	const float fy = py - floorf(py);

	Color result = tex[0][0] * (1 - fx) * (1 - fy) + 
								 tex[1][0] * fx * (1 - fy) +
								 tex[0][1] * (1 - fx) * fy + 
								 tex[1][1] * fx * fy;

	return result;
}

Color Texture::sampleNearestRepeat(float u, float v) const
{
	int px = (mWidth + static_cast<int>(u) % mWidth) % mWidth;
	int py = (mHeight + static_cast<int>(v) % mHeight) % mHeight;

	assert(px >= 0);
	assert(py >= 0);

	return mData[py * mWidth + px];
}