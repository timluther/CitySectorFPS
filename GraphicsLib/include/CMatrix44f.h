#ifndef CMatrix44f_H_INCLUDED
#define CMatrix44f_H_INCLUDED

#include "CMatrix44f.h"

class CVector3f;

class CMatrix44f
{
public:
	CMatrix44f();	
	CMatrix44f(const CMatrix44f &other);

	void Ortho(float argLeft, float argRight, float argBottom, float argTop,float argZNear, float argZFar); ///<Build an orthographic projection matrix
	void Ortho2D(float argLeft, float argRight, float argBottom, float argTop);                             ///<Build an orthographic projection matrix discounting depth
	
	void PerspectiveLH(float argFov, float argAspect, float argZNear, float argZFar);          // Build perspective projection matrix LH
	void PerspectiveRH(float argFov, float argAspect, float argZNear, float argZFar);          // Build perspective projection matrix RH	

	void Set(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23,float m30, float m31, float m32, float m33);

	float m_data[16];
};

#endif
