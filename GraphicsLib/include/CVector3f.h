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

    float x, y, z;
};

#endif
