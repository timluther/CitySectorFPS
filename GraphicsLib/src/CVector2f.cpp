#include "CVector2f.h"
#include <math.h>

CVector2f::CVector2f()
	:x(0.0f)
	, y(0.0f)
{
}

void CVector2f::add(float x_, float y_)
{
	x += x_;
	y += y_;
}

CVector2f CVector2f::normal()const
{
	const float len = length();
	if (len != 0.f)
	{
		const float inv_len = 1.0f / len;
		return CVector2f(x*inv_len, y*inv_len);
	}
	else
	{
		return CVector2f{ 0, 0 };
	}
}

const CVector2f & CVector2f::normalise()
{
	const float len = length();
	if (len != 0.f)
	{
		const float inv_len = 1.0f / len;
		x *= inv_len; y *= inv_len;
	}
	return *this;
}

float CVector2f::length()const
{
	return sqrtf(x*x + y*y);
}

CVector2f::CVector2f(float x_, float y_)
	:x(x_)
	, y(y_)
{
}

CVector2f::CVector2f(const CVector2f &other)
	: x(other.x)
	, y(other.y)
{
}

void CVector2f::add(const CVector2f &rhs)
{
	x += rhs.x;
	y += rhs.y;
}
