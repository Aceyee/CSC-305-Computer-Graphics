#pragma once
#include "object.h"
#include "common.h"
#include <algorithm>

class Sphere : public Object
{
    Vector3 Center; //center
    float Radius;   //radius
    int flag;       //falg
public:
    Sphere(Vector3 Center_in, float Radius_in,int flag_in)
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
        if (discriminant > 0)   //intersect with sphere
        {
            float d = sqrt(discriminant);
            t = v - d;
        }

        if (t > 0) {   // t < 0 means at the back side of sphere
            *t_out = t;
            Vector3 IntersectionPoint = MultiplyScalar(Direction, t);
            IntersectionPoint = Add(IntersectionPoint, Origin);
            Vector3 SurfaceNormal = Minus(IntersectionPoint, Center);
            (*normal_out) = Normalize(SurfaceNormal);
            return true;
        }else{
            return false;
        }
    }
    /* Specular component */
    Vector3 SpecularShade(Vector3 Normal, Vector3 LightVector,Vector3 V,Vector3 PixelColour){
        Vector3 tmp = MultiplyScalar(Normal,2 * DotProduct(Normal,LightVector));   //2(N.L)N
        Vector3 R = Minus(tmp,LightVector);    //2(N.L)N - L
        R = Normalize(R);
        float rv = std::min(0.0f,DotProduct(R,V));  //(R.V)
        if(rv < 0) rv = rv*-1;
        float specular = pow(rv,60); //specular power

        if(specular > 0){
            Vector3 PixelSpecularColour = MultiplyScalar(SpecularColourSphere,specular);
            PixelColour = Add(PixelColour,PixelSpecularColour);  //ambient + diffuse + specular
        }
        return PixelColour;
    }

    virtual Pixel DiffuseShade(int flag,Vector3 Direction,Vector3 Surface, Vector3 Normal,std::vector<Object *> pObjectList)
    {

        Pixel shade;    //define pixel
        Vector3 LightVector = Minus(Light, Surface);    //light vector 1
        Vector3 LightVector2 = Minus(Light2,Surface);   //light vector 2
        LightVector = Normalize(LightVector);
        LightVector2 = Normalize(LightVector2);
        float DiffuseTerm = DotProduct(LightVector, Normal);    //diffuse term = Cos(o)
        float DiffuseTerm2 = DotProduct(LightVector2, Normal);
        Vector3 V =Minus(Surface,Camera);//vector from camera to intersection point
        V = Normalize(V);
        Vector3 PixelColour = AmbientColour; //set to ambient colour initially
        if (DiffuseTerm > 0)    // Cos(o) > 0 calculate diffuse shading
        {
            Vector3 PixelDiffuseColour = MultiplyScalar(DiffuseColour, DiffuseTerm);
            PixelColour = Add(PixelColour, PixelDiffuseColour);
        }
        if (DiffuseTerm2 > 0)
        {
            Vector3 PixelDiffuseColour = MultiplyScalar(DiffuseColour, DiffuseTerm2);
            PixelColour = Add(PixelColour, PixelDiffuseColour);
        }
        PixelColour = MultiplyScalar(PixelColour,0.5); // divided by 2

        /* Specular component */
        Vector3 specular1 = SpecularShade(Normal,LightVector,V,PixelColour);
        Vector3 specular2 = SpecularShade(Normal,LightVector2,V,PixelColour);

        PixelColour = MultiplyScalar(Add(specular1,specular2),0.5);
        SetColor(shade, PixelColour);
        return shade;
    }
};
