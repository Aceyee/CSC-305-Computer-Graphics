#pragma once
#include "object.h"

class Sphere : public Object
{
public:
    Sphere(Vector3 Center_in, float Radius_in)
    {
        Center = Center_in;
        Radius = Radius_in;
        type = 1;
    }

    virtual int getType(){
        return type;
    }

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

    Vector3 SpecularShade(Vector3 Normal, Vector3 LightVector,Vector3 V,Vector3 PixelColour){
        Vector3 minor = MultiplyScalar(Normal,2 * DotProduct(Normal,LightVector));   //2(N.L)N
        Vector3 R = Minus(minor,LightVector);    //2(N.L)N - L
        R = Normalize(R);
        float rv = std::min(0.0f,DotProduct(R,V));  //(R.V)
        if(rv < 0) {
            rv = -rv;
        }
        float specular = pow(rv,50); //(R.V)^n
        Vector3 PixelSpecularColour = MultiplyScalar(SpecularColourSphere,specular);
        PixelColour = Add(PixelColour,PixelSpecularColour);  //ambient + diffuse + specular

        return PixelColour;
    }
    virtual Vector3 Color(){
        return Vector3(250, 62.5, 62.5);
    }

    virtual Pixel DiffuseShade(int type, Vector3 Origin, Vector3 Surface, Vector3 Normal)
    {

        Pixel shade;
        Vector3 DiffuseColour = Color();
        Vector3 LightVector = Minus(Light, Surface);
        LightVector = Normalize(LightVector);
        float DiffuseTerm = DotProduct(LightVector, Normal);
        Vector3 PixelColour = AmbientColour;
        if (DiffuseTerm > 0)
        {

            Vector3 PixelDiffuseColour = MultiplyScalar(DiffuseColour, DiffuseTerm);
            PixelColour = Add(PixelColour, PixelDiffuseColour);
        }
        PixelColour = MultiplyScalar(PixelColour, 2);

        Vector3 Vector =Minus(Surface,Origin);
        Vector = Normalize(Vector);

        Vector3 specular = SpecularShade(Normal,LightVector,Vector,PixelColour);
        PixelColour = MultiplyScalar(specular, 0.5);
        SetColor(shade, PixelColour);
        return shade;
    }

    Vector3 Center;
    float Radius;
    int type;
};
