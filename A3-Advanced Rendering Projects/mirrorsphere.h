#pragma once
#include <math.h>
#include <ctgmath>
#include <cmath>
#include "object.h"
#include "common.h"
#include <iostream>

class mirrorsphere : public Object
{
    Vector3 Center;
    float Radius;
    int flag;

public:
    mirrorsphere(Vector3 Center_in, float Radius_in,int flag_in)
    {
        Center = Center_in;
        Radius = Radius_in;
        flag = flag_in;
    }
    virtual Vector3 getCenter(){return Center;}
    virtual float getRadius(){return Radius;}
    virtual int getflag() {return flag;}

    virtual bool Intersect(Vector3 Origin, Vector3 Direction,
                           float *t_out, Vector3 *normal_out)
    {
        Vector3 EO = Minus(Center, Origin);
        float v = DotProduct(EO, Direction);
        float RadiusSquare = Radius * Radius;
        float EO_Square = DotProduct(EO, EO);
        float discriminant = RadiusSquare - (EO_Square - v * v);

        float t = -1;
        if (discriminant > 0)
        {
            float d = sqrt(discriminant);
            t = v - d;
        }

        if (t > 0) {
            *t_out = t;
            Vector3 IntersectionPoint = MultiplyScalar(Direction, t);
            IntersectionPoint = Add(IntersectionPoint, Origin);
            Vector3 SurfaceNormal = Minus(IntersectionPoint, Center);
            (*normal_out) = Normalize(SurfaceNormal);
            return true;
        } else
        {
            return false;
        }
    }

    virtual Pixel DiffuseShade(int flag,Vector3 Direction,Vector3 Intersection, Vector3 Normal,std::vector<Object *> pObjectList)
    {

        Pixel shade;
        int index = 0;
        bool HasIntersection = false;
        float t_min = 999999;
        Vector3 Normal_min;

        /*reflect ray 2(n.v)n - v */
        Vector3 v = Direction;
        v = MultiplyScalar(v,-1);
        Vector3 r = MultiplyScalar(Normal,2 * DotProduct(Normal,v));
        r = Minus(r,v);
        r = Normalize(r); //new direction

        /*****************************************/

        /* The rest is just same intersection and shading color BUT with different camera position */
        for(int i =0;i<pObjectList.size();++i){
            if(pObjectList[i]->getflag() != 2){
                float t2;
                Vector3 normal2;
                bool DoesIntersect = pObjectList[i]->Intersect(Intersection, r,
                                                             &t2, &normal2);
                if (DoesIntersect)
                {
                    HasIntersection = true;
                    if (t_min > t2)
                    {
                        index = i;
                        t_min = t2;
                        Normal_min = normal2;
                    }
                }
            }
        }

        if (HasIntersection)
        {
            Vector3 intersection = MultiplyScalar(r, t_min);
            intersection = Add(intersection, Intersection);
            shade = pObjectList[index]->DiffuseShade(pObjectList[index]->getflag(),r,intersection, Normal_min,pObjectList);
        }//if t > 0
        else //No Intersection, set background colour
        {
            SetColor(shade, BackgroundColor);
        }

        return shade;
    }


};
