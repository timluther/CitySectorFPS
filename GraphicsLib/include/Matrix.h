//
// Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef SAMPLE_UTIL_MATRIX_H
#define SAMPLE_UTIL_MATRIX_H

#include "CVector3f.h"
#include "CVector4f.h"
struct Matrix4
{
    float data[16];

    Matrix4();
    Matrix4(float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33);

	CVector3f	column(unsigned int idx) const;
	const CVector3f	&row(unsigned int idx) const;
    static Matrix4 identity();
    static Matrix4 rotate(float angle, const CVector3f &p);
    static Matrix4 translate(const CVector3f &t);
    static Matrix4 scale(const CVector3f &s);
    static Matrix4 frustum(float l, float r, float b, float t, float n, float f);
    static Matrix4 perspective(float fov, float aspectRatio, float n, float f);
    static Matrix4 ortho(float l, float r, float b, float t, float n, float f);
    static Matrix4 rollPitchYaw(float roll, float pitch, float yaw);

    static Matrix4 invert(const Matrix4 &mat);
    static Matrix4 transpose(const Matrix4 &mat);
    static CVector3f transform(const Matrix4 &mat, const CVector3f &pt);
    static CVector4f transform(const Matrix4 &mat, const CVector4f &pt);
};

Matrix4 operator*(const Matrix4 &a, const Matrix4 &b);
Matrix4 &operator*=(Matrix4 &a, const Matrix4 &b);
Matrix4 operator*(const Matrix4 &a, float b);
Matrix4 &operator*=(Matrix4 &a, float b);

CVector3f operator*(const Matrix4 &a, const CVector3f &b);
CVector4f operator*(const Matrix4 &a, const CVector4f &b);

//Vector4 operator*(const Matrix4 &a, const Vector4 &b);

bool operator==(const Matrix4 &a, const Matrix4 &b);
bool operator!=(const Matrix4 &a, const Matrix4 &b);

#endif // SAMPLE_UTIL_MATRIX_H
