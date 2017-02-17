#pragma once

#include "common.h"

class Object
{
public:
    virtual bool Intersect(Vector3 Origin, Vector3 Direction,
                           float * t, Vector3 * normal) = 0;

    virtual Pixel DiffuseShade(int type, Vector3 Origin, Vector3 Surface, Vector3 Normal) = 0;

    virtual Vector3 Color()=0;

    virtual int getType()=0;
};
