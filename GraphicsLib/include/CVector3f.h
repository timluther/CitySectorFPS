#ifndef CVECTOR3F_H
#define CVECTOR3F_H

class CVector3f
{
    public:

    CVector3f();

    CVector3f(float x_, float y_, float z_);

    CVector3f(const CVector3f &other);

    CVector3f cross(const CVector3f &other)const;

    CVector3f normal()const;

    const CVector3f & normalise();

    float length()const;

    void add(CVector3f *thisptr, float x_, float y_, float z_);

    void add(float x_, float y_, float z_);

    void add(const CVector3f &rhs);

	CVector3f operator *(const CVector3f &other) const { return CVector3f(x * other.x, y * other.y, z * other.z); }
	CVector3f operator +(const CVector3f &other) const { return CVector3f(x + other.x, y + other.y, z + other.z); }
	CVector3f operator -(const CVector3f &other) const { return CVector3f(x - other.x, y - other.y, z - other.z); }

	CVector3f operator *(float v) const { return CVector3f(x * v, y * v, z * v); }
	CVector3f operator +(float v) const { return CVector3f(x + v, y + v, z + v); }
	CVector3f operator -(float v) const { return CVector3f(x - v, y - v, z - v); }

	const CVector3f &operator *=(const CVector3f &other) { x *= other.x; y *= other.y; z *= other.z; return *this; }
	const CVector3f &operator +=(const CVector3f &other) { x += other.x; y += other.y; z += other.z; return *this; }
	const CVector3f &operator -=(const CVector3f &other) { x -= other.x; y -= other.y; z -= other.z; return *this; }

	const CVector3f &operator *=(float v) { x *= v; y *= v; z *= v; return *this; }
	const CVector3f &operator +=(float v) { x += v; y += v; z += v; return *this; }
	const CVector3f &operator -=(float v) { x -= v; y -= v; z -= v; return *this; }

	float operator [] (size_t idx) const {return (&x)[idx];}
	float &operator [] (size_t idx) { return (&x)[idx]; }
	
    float x, y, z;
};

#endif
