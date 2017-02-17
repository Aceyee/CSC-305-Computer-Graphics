#pragma once

#include "common.h"

class Object
{
public:
    /* if ray intersects with one of the objects */
    virtual bool Intersect(Vector3 Origin, Vector3 Direction,
                           float * t, Vector3 * normal) = 0;
    /* shading part */
    virtual Pixel DiffuseShade(int flag,Vector3 Direction,Vector3 Surface, Vector3 Normal,std::vector<Object *> pObjectList) = 0;
    virtual Vector3 getCenter() = 0; //return center of sphere
    virtual float getRadius() = 0;   //return the radius of sphere
    virtual int getflag() = 0;       //flag
};
