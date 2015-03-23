/********************************************//**
 * \copyright (c) 2014 
 * \file   	CVector4f.cpp
 * \Author 	Tim Lewis (tim@totga-games.com)
 * \see    	Project: Anthracite
 * \date   	11-09-2014
 * \brief  	CVector4f
 * CVector4f is a four dimensional vector class
 ***********************************************/
#include "CVector4f.h"
#include "CVector3f.h"
#include <math.h>

CVector4f::CVector4f()
{
}


CVector3f &CVector4f::xyz() {return *(CVector3f*)this;}
const CVector3f &CVector4f::xyz() const {return *(CVector3f*)this;}


void CVector4f::set_dword(unsigned int dword)
{
	const float inv = 1.0f / 255.0f;
	x = (float)(dword >> 24)		 * inv;
	y = (float)((dword >> 16)& 0xFF) * inv;
	z = (float)((dword >> 8) & 0xFF) * inv;
	w = (float)(dword  & 0xFF)		 * inv;
}

unsigned int CVector4f::get_dword() const
{
	enum indices{I0 = 0, I1, I2, I3};
	const float *v= &x;
	return  (((unsigned char)::floorf(v[I3]*255.0f))<<24)|
			(((unsigned char)::floorf(v[I2]*255.0f))<<16)|
			(((unsigned char)::floorf(v[I1]*255.0f))<<8)|
			(((unsigned char)::floorf(v[I0]*255.0f)));
}

CVector4f CVector4f::normal() const 
{
	float len = sqrtf(x*x + y*y + z*z + w*w); 
	return *this / len;
}