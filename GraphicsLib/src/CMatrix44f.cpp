
#include "CMatrix44f.h"



CMatrix44f::CMatrix44f()
{
	m_data[0] = 1.0f;
	m_data[1] = 0.0f;
	m_data[2] = 0.0f;
	m_data[3] = 0.0f;

	m_data[4] = 0.0f;
	m_data[5] = 1.0f;
	m_data[6] = 0.0f;
	m_data[7] = 0.0f;

	m_data[8] = 0.0f;
	m_data[9] = 0.0f;
	m_data[10] = 1.0f;
	m_data[11] = 0.0f;
	m_data[12] = 0.0f;
	m_data[13] = 0.0f;
	m_data[14] = 0.0f;
	m_data[15] = 1.0f;
}
CMatrix44f::CMatrix44f(const CMatrix44f &other)
{
	for (size_t i = 0; i < 16; ++i)
		m_data[i] = other.m_data[i];
}

void CMatrix44f::Ortho(float argLeft, float argRight, float argBottom, float argTop,
		float argZNear, float argZFar)
{
	CMatrix44f tempMat;
	tempMat.m_data[0] = 2 / (argRight - argLeft);
	tempMat.m_data[5] = 2 / (argTop - argBottom);
	tempMat.m_data[10] = -2 / (argZFar - argZNear);
	tempMat.m_data[12] = -(argRight + argLeft) / (argRight - argLeft);
	tempMat.m_data[13] = -(argTop + argBottom) / (argTop - argBottom);
	tempMat.m_data[14] = -(argZFar + argZNear) / (argZFar - argZNear);

	PostMultiplyWith(tempMat);
}

void CMatrix44f::Ortho2D(float argLeft, float argRight, float argBottom, float argTop)
{
	Ortho(argLeft, argRight, argBottom, argTop, -100, 100);
}

void CMatrix44f::Set(float m00, float m01, float m02, float m03,
					float m10, float m11, float m12, float m13,
					float m20, float m21, float m22, float m23,
					float m30, float m31, float m32, float m33)
{
	m_data[0] = m00; m_data[1]  = m01; m_data[2]   = m02; m_data[3]   = m03;
	m_data[4] = m10; m_data[5]  = m11; m_data[6]   = m12; m_data[7]   = m13;
	m_data[8] = m20; m_data[9]  = m21; m_data[10]  = m22; m_data[11]  = m23;
	m_data[12]= m30; m_data[13] = m31; m_data[14]  = m32; m_data[15]  = m33;
}

/********************************************//**
* \brief 	CMatrix44f::PerspectiveRH
* \param 	argFov[in]      field of view (in degrees)
* \param 	argAspect[in]   The aspect ratio of the viewport
* \param 	argZNear[in]    The near Z value (all values in front of this are clipped)
* \param 	argZFar[in] 	The far Z value (all values beyond this are clipped)
* \details 	This is a \em Right \em Handed perspective matrix
			this calles 'FrustumRH' to populate the matrix itself
***********************************************/
void CMatrix44f::PerspectiveRH(float argFov, float argAspect, float argZNear, float argZFar)
{	
	float range = (float) (argZNear * tan((argFov / 2) * DEG_TO_RAD));
	
	FrustumRH(-range * argAspect, range * argAspect, -range, range, argZNear, argZFar);
}

/********************************************//**
* \brief 	CMatrix44f::PerspectiveLH
* \param 	argFov[in]      field of view (in degrees)
* \param 	argAspect[in]   The aspect ratio of the viewport
* \param 	argZNear[in]    The near Z value (all values in front of this are clipped)
* \param 	argZFar[in] 	The far Z value (all values beyond this are clipped)
* \details 	This is a <a href="http://en.wikipedia.org/wiki/Right-hand_rule">  \em Left \em Handed </a> perspective matrix 
			this calles 'FrustumRH' to populate the matrix itself
***********************************************/
void CMatrix44f::PerspectiveLH(float argFov, float argAspect, float argZNear, float argZFar)
{	
	float range = (float) (argZNear * tan((argFov / 2) * DEG_TO_RAD));
	
	FrustumLH(-range * argAspect, range * argAspect, -range, range, argZNear, argZFar);
}


/********************************************//**
* \brief 	CMatrix44f::FrustumRH
* \param 	l[in] Left edge of the view port 
* \param 	r[in] Right edge of the view port 
* \param 	b[in] Bottom edge of the view port 
* \param 	t[in] Top edge  of the view port 
* \param 	n[in] Near Z plane
* \param 	f[in] Right Z plane
* \details This is a <a href="http://en.wikipedia.org/wiki/Right-hand_rule">  \em Right \em Handed </a> perspective matrix builder.
* Expressed with mathematical notation and using the above parameters, the matrix is:<br/><br/>
\f$\begin{bmatrix}n2 / (r - l) & 0 & 0 & 0 \\0 & n2 / (t - b) & 0 & 0 \\r + l / (r - l) & (t + b) & - (f + n) / (f - n) & -1         \\0 & 0 & -(2 * f * n) / (f-n) & 0\end{bmatrix}\f$
***********************************************/
void CMatrix44f::FrustumRH(float l, float r, float b, float t, float n, float f)
{
	float n2 = n * 2;
	float r_l = r - l;
	float t_b = t - b;
	float f_n = f - n;

	Set(n2 / r_l, 0, 0, 0, 
		0, n2 / t_b, 0, 0, 
		(r + l) / r_l, (t + b) / t_b, -(f + n) / f_n, -1, 
		0, 0, -(2.0f * f * n) / (f_n), 0);
}


/********************************************//**
* \brief 	CMatrix44f::FrustumLH
* \param 	l[in] Left edge of the view port 
* \param 	r[in] Right edge of the view port 
* \param 	b[in] Bottom edge of the view port 
* \param 	t[in] Top edge  of the view port 
* \param 	n[in] Near Z plane
* \param 	f[in] Right Z plane
* \details This is a <a href="http://en.wikipedia.org/wiki/Right-hand_rule">  \em Left \em Handed </a> perspective matrix builder.
* Expressed with mathematical notation and using the above parameters, the matrix is:<br/><br/>
\f$\begin{bmatrix}n2 / (r - l) & 0 & 0 & 0 \\0 & n2 / (t - b) & 0 & 0 \\r + l / (r - l) & (t + b) & - (f + n) / (f - n) & 1         \\0 & 0 & (2 * f * n) / (f-n) & 0\end{bmatrix}\f$
***********************************************/

void CMatrix44f::FrustumLH(float l, float r, float b, float t, float n, float f)
{
	float n2 = n * 2;
	float r_l = r - l;
	float t_b = t - b;
	float f_n = n - f;

	Set(n2 / r_l, 0, 0, 0, 
		0, n2 / t_b, 0, 0, 
		(r + l) / r_l, (t + b) / t_b, -(f + n) / f_n, 1, 
		0, 0, (2.0f * f * n) / (f_n), 0);
}
