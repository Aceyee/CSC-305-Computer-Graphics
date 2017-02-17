#pragma once
#include "object.h"

class Floor : public Object
{
public:
    Floor(Vector3 Point1, Vector3 Point2, Vector3 Point3)
    {
        p1 = Point1;
        p2 = Point2;
        p3 = Point3;
        type = 0;
    }

    virtual int getType(){
        return type;
    }

    virtual bool Intersect(Vector3 Origin, Vector3 Direction,
                           float * t_out, Vector3 * normal)
    {
        Vector3 v1 = Minus(p2, p1);
        Vector3 v2 = Minus(p2, p3);
        Vector3 n = CrossProduct(v1, v2);
        n=Normalize(n);
        Vector3 bu = Minus(p2, Origin);

        float t = DotProduct(n, bu)/(DotProduct(n,Direction));

        if(t>0){
            *t_out = t;
            Vector3 IntersectionPoint = MultiplyScalar(Direction, t);
            IntersectionPoint = Add(IntersectionPoint, Origin);

            int cp1 = DotProduct(CrossProduct(Minus(p2, p1), Minus(IntersectionPoint, p1)),n);
            int cp2 = DotProduct(CrossProduct(Minus(p3, p2), Minus(IntersectionPoint, p2)),n);
            int cp3 = DotProduct(CrossProduct(Minus(p1, p3), Minus(IntersectionPoint, p3)),n);

            //Vector3 SurfaceNormal = Minus(IntersectionPoint, FloorCenter(p1,p2,p3));
            Vector3 SurfaceNormal = Minus(IntersectionPoint, Origin);
            SurfaceNormal = MultiplyScalar(SurfaceNormal, -1);
            (*normal) = Normalize(SurfaceNormal);

            if(cp1>0&&cp2>0&&cp3>0){
                return true;
            }else if(cp1<0&&cp2<0&&cp3<0){
                return true;
            }else{
                return false;
            }

        }else{
            return false;
        }
    }

    virtual Vector3 Color(){
        return Vector3(25, 150, 160);
    }

    virtual Pixel DiffuseShade(int type, Vector3 Origin, Vector3 Surface, Vector3 Normal)
    {
        //These colours will give the sphere a red appearance
        Pixel shade;
        Vector3 DiffuseColour = Color();
        Vector3 LightVector = Minus(Light, Surface);
        LightVector = Normalize(LightVector);
        float DiffuseTerm = 1.5*DotProduct(LightVector, Normal);
        Vector3 PixelColour = AmbientColour;
        if (DiffuseTerm > 0)
        {
            Vector3 PixelDiffuseColour = MultiplyScalar(DiffuseColour, DiffuseTerm);
            PixelColour = Add(PixelColour, PixelDiffuseColour);
        }
        SetColor(shade, PixelColour);
        return shade;
    }
    Vector3 p1;
    Vector3 p2;
    Vector3 p3;
    int type;
};
