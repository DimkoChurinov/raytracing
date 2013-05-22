#ifndef GEOMETRY_VECTOR3D_H
#define GEOMETRY_VECTOR3D_H
#include <math.h>

#include "precision.h"

class Vec3D
{
	friend Vec3D operator+(const Vec3D& lh, const Vec3D& rh);

	friend Vec3D operator-(const Vec3D& lh, const Vec3D& rh);

	friend Vec3D operator*(const Vec3D& lh, float value);

	friend Vec3D operator*(float value, const Vec3D& rh);

	friend Vec3D operator/(const Vec3D& lh, float value);

	friend Vec3D operator-(const Vec3D& v);

	friend float dot(const Vec3D& lh, const Vec3D& rh);

	friend Vec3D cross(const Vec3D& lh, const Vec3D& rh);

	friend float length(const Vec3D& v);

	friend float length2(const Vec3D& v);

	friend Vec3D scale3D(const Vec3D& lh, const Vec3D& rh);

public:

	Vec3D()
		: mX(0.f),
			mY(0.f),
			mZ(0.f)
	{

	}

	Vec3D(float x, float y, float z)
		: mX(x), mY(y), mZ(z)
	{

	}

	float x() const
	{
		return mX;
	}

	float y() const
	{
		return mY;
	}

	float z() const
	{
		return mZ;
	}

	void setX(float x) 
	{
		mX = x;
	}

	void setY(float y)
	{
		mY = y;
	}

	void setZ(float z)
	{
		mZ = z;
	}

	void setXYZ(float x, float y, float z)
	{
		mX = x; mY = y; mZ = z;
	}

	Vec3D& operator+=(const Vec3D& rh)
	{
		mX += rh.mX;
		mY += rh.mY;
		mZ += rh.mZ;
		return *this;
	}

	Vec3D& operator-=(const Vec3D& rh)
	{
		mX -= rh.mX;
		mY -= rh.mY;
		mZ -= rh.mZ;
		return *this;
	}

	Vec3D& operator*=(float value)
	{
		mX *= value;
		mY *= value;
		mZ *= value;
		return *this;
	}

	Vec3D& operator/=(float value)
	{
		mX /= value;
		mY /= value;
		mZ /= value;
		return *this;
	}

	void normalize()
	{
		float len = length(*this);
		if (len != 0.f)
			*this /= len;
	}

  Vec3D& toUnit()
	{
		normalize();
		return *this;
	}

	Vec3D inverse() const
	{
		return Vec3D(1.f / mX, 1.f / mY, 1.f / mZ);
	}

	Vec3D absolute() const
	{
		return Vec3D(fabs(mX), fabs(mY), fabs(mZ));
	}

private:
	float mX, mY, mZ;
};

inline Vec3D operator+(const Vec3D& lh, const Vec3D& rh)
{
	return Vec3D(lh.mX + rh.mX, lh.mY + rh.mY, lh.mZ + rh.mZ);
}

inline Vec3D operator-(const Vec3D& lh, const Vec3D& rh)
{
	return Vec3D(lh.mX - rh.mX, lh.mY - rh.mY, lh.mZ - rh.mZ);
}

inline Vec3D operator*(const Vec3D& lh, float value)
{
	return Vec3D(lh.mX * value, lh.mY * value, lh.mZ * value);
}

inline Vec3D operator*(float value, const Vec3D& rh)
{
	return Vec3D(rh.mX * value, rh.mY * value, rh.mZ * value);
}

inline Vec3D operator/(const Vec3D& lh, float value)
{
	return Vec3D(lh.mX / value, lh.mY / value, lh.mZ / value);
}

inline Vec3D operator-(const Vec3D& lh)
{
	return Vec3D(-lh.mX, -lh.mY, -lh.mZ);
}

inline float dot(const Vec3D& lh, const Vec3D& rh)
{
	return lh.mX * rh.mX + lh.mY * rh.mY + lh.mZ * rh.mZ;
}

inline Vec3D cross(const Vec3D& lh, const Vec3D& rh)
{
	return Vec3D(lh.mY * rh.mZ - lh.mZ * rh.mY,
									lh.mZ * rh.mX - lh.mX * rh.mZ,
									lh.mX * rh.mY - lh.mY * rh.mX);
}

inline float length(const Vec3D& v)
{
	float len = length2(v);

	if (len < FLOAT_ZERO)
		return 0.f;
	if (len == 1.f)
		return 1.f;

	return sqrtf(dot(v, v));
}

inline float length2(const Vec3D& v)
{
	return dot(v, v);
}

inline Vec3D scale3D(const Vec3D& lh, const Vec3D& rh)
{
	return Vec3D(lh.mX * rh.mX, lh.mY * rh.mY, lh.mZ * rh.mZ);
}

#endif