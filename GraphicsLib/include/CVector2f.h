#ifndef CVECTOR2F_H
#define CVECTOR2F_H

class CVector2f
{
    public:

    CVector2f();

    CVector2f(float x_, float y_);

    CVector2f(const CVector2f &other);

    
    CVector2f normal()const;

    const CVector2f & normalise();

    float length()const;

    void add(float x_, float y_);

    void add(const CVector2f &rhs);

    float x, y;
};

#endif
