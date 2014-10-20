#include "CVector3f.h"
#include <math.h>

CVector3f::CVector3f()
        :x(0.0f)
        ,y(0.0f)
        ,z(0.0f)
{
}

void CVector3f::add(float x_, float y_, float z_)
{
    x += x_;
    y += y_;
    z += z_;
}

CVector3f CVector3f::cross(const CVector3f &other)const
{
    return CVector3f
    (
        y*other.z - z*other.y,
        z*other.x - x*other.z,
        x*other.y - y*other.x
    );

}

CVector3f CVector3f::normal()const
{
    const float len = length();
    if (len != 0.f)
    {
        const float inv_len = 1.0f/len;
        return CVector3f(x*inv_len, y*inv_len, z*inv_len);
    }
    else
    {
        return CVector3f{0, 0, 0};
    }
}

const CVector3f & CVector3f::normalise()
{
    const float len = length();
    if (len != 0.f)
    {
        const float inv_len = 1.0f/len;
        x*=inv_len; y*=inv_len; z*=inv_len;
    }
    return *this;
}

float CVector3f::length()const
{
    return sqrtf(x*x + y*y + z*z);
}

void CVector3f::add(CVector3f *thisptr, float x_, float y_, float z_)
{
    thisptr -> x += x_;
    thisptr -> y += y_;
    thisptr -> z += z_;
}

CVector3f::CVector3f(float x_, float y_, float z_)
    :x(x_)
    ,y(y_)
    ,z(z_)
{
}

CVector3f::CVector3f(const CVector3f &other)
    :x(other.x)
    ,y(other.y)
    ,z(other.z)
{
}

void CVector3f::add(const CVector3f &rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
}
