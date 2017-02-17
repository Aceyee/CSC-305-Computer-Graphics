#pragma once
#include "object.h"
#include "common.h"
#include <iostream>

class triangle : public Object
{
    /* Three Vertex of a triangle */
    Vector3 Vertex3;
    Vector3 Vertex2;
    Vector3 Vertex1;
    int flag;
public:
    triangle(Vector3 Vertex3_in, Vector3 Vertex2_in,Vector3 Vertex1_in,int flag_in)
    {
        Vertex3 = Vertex3_in;
        Vertex2 = Vertex2_in;
        Vertex1 = Vertex1_in;
        flag = flag_in;
    }
	triangle(){
		Vertex3 = Vector3(0, 0, 0);
		Vertex2 = Vector3(0, 0, 0);
		Vertex1 = Vector3(0, 0, 0);
		flag = 10;
	}
    virtual Vector3 getCenter(){return Vertex3;}
	virtual float getRadius(){ return 0; }
    virtual int getflag() {return flag;}

    virtual bool Intersect(Vector3 Origin, Vector3 Direction,
                           float *t, Vector3 *normal)
    {
         Vector3 trianglenormal;
        /* get the normal vector of each triangle */
        if(flag == 10||flag == 25){
			trianglenormal = Vector3(0, 1, 0);
		}
		else if (flag == 11 || flag == 23){
			trianglenormal = Vector3(1, 0, 0);
		}
		else if (flag == 12 || flag == 24){
			trianglenormal = Vector3(0, 0, -1);
		}
		else if (flag == 13 || flag == 21){
			trianglenormal = Vector3(-1, 0, 0);
		}
		else if (flag == 14 || flag == 22){
			trianglenormal = Vector3(0, 0, 1);
		}
		if (flag == 15 || flag == 20){
			trianglenormal = Vector3(0, -1, 0);
		}
        
        (*normal) = trianglenormal;
        Vector3 NminusP = Minus(Origin,Vertex3);  //(o-p)
        float tmp = -1 * DotProduct(NminusP,trianglenormal); //(o-p).n
        float D_N = DotProduct(Direction,trianglenormal); // d.n
        (*t) = tmp / D_N; // t
        //bool inside = false;
        Vector3 Intersectiontmp = MultiplyScalar(Direction, tmp / D_N);
        Intersectiontmp = Add(Intersectiontmp, Origin);
        if((*t) > 0 ){
            int line1,line2,line3;
                line1 = DotProduct(trianglenormal,CrossProduct(Minus(Vertex3,Vertex1),Minus(Intersectiontmp,Vertex1)));
                line2 = DotProduct(trianglenormal,CrossProduct(Minus(Vertex2,Vertex3),Minus(Intersectiontmp,Vertex3)));
                line3 = DotProduct(trianglenormal,CrossProduct(Minus(Vertex1,Vertex2),Minus(Intersectiontmp,Vertex2)));
            /* test if the intersection point is within the triangle; they must have the same sign */
            if(line1 >= 0 && line2 >= 0 && line3 >= 0){
                return true;
            }
            if(line1 <= 0 && line2 <= 0 && line3 <= 0){
                return true;
			}
			else{
				return false;
			}
        }else{
            return false;
        }
    }

    Vector3 addcolour(int flag,float PlaneDiffuseTerm,Vector3 PixelColour,int blackOrWhite){
        /* each wall with different colour */
		if (flag == 11 || flag == 23){ //left
            Vector3 PixelDiffuseColour = MultiplyScalar(left, PlaneDiffuseTerm);
            PixelColour = Add(PixelColour,PixelDiffuseColour);
		}
		else if (flag == 12 || flag == 24){ //back
			Vector3 PixelDiffuseColour = MultiplyScalar(back, PlaneDiffuseTerm);
			PixelColour = Add(PixelColour, PixelDiffuseColour);
		}
		else if (flag == 13 || flag == 21 || flag == 25){ //right
            Vector3 PixelDiffuseColour = MultiplyScalar(right, PlaneDiffuseTerm);
            PixelColour = Add(PixelColour,PixelDiffuseColour);
		}
		else if (flag == 14 || flag == 22){ //front
			Vector3 PixelDiffuseColour = MultiplyScalar(top, PlaneDiffuseTerm);
			PixelColour = Add(PixelColour, PixelDiffuseColour);
		}
		else if (flag == 15 || flag == 20){ //top
            Vector3 PixelDiffuseColour = MultiplyScalar(top, PlaneDiffuseTerm);
            PixelColour = Add(PixelColour,PixelDiffuseColour);
		}
		else if (flag == 10 ){//bot
            if (blackOrWhite== -1){
                Vector3 PixelDiffuseColour = MultiplyScalar(White, PlaneDiffuseTerm);
                PixelColour = Add(PixelColour,PixelDiffuseColour);
                //SetColor(shade, PixelColour);
            }else{
                Vector3 PixelDiffuseColour = MultiplyScalar(yellow, PlaneDiffuseTerm);
                PixelColour = Add(PixelColour,PixelDiffuseColour);
                //SetColor(shade, PixelColour);
            }
        }
        return PixelColour;
    }

    virtual Pixel DiffuseShade(int flag,Vector3 Direction,Vector3 Surface, Vector3 Normal,std::vector<Object *> pObjectList)
    {
             Pixel shade;
             Vector3 PixelColour = AmbientColour;
             /* Checker board */
             int blackOrWhite = -1;
             float result = abs((int)Surface.x/50%2);
             float result1 = abs((int)Surface.z/50%2);
             //std::cout << result << std::endl;
             if(result == result1  ){
                 blackOrWhite = 1;
             }
             if(Surface.x < 0 ){
                blackOrWhite = blackOrWhite * -1;
             }
             if(Surface.z < 0){
                 blackOrWhite = blackOrWhite * -1;
             }

             Vector3 PlaneLightVector =Minus(Light, Surface); //L
             Vector3 PlaneLightVector2 =Minus(Light2, Surface); //L
             PlaneLightVector = Normalize(PlaneLightVector);
             PlaneLightVector2 = Normalize(PlaneLightVector2);
             Vector3 tmp = Surface;
             Surface = Normalize(Surface);
             float PlaneDiffuseTerm = DotProduct(Normal,PlaneLightVector);
             float PlaneDiffuseTerm2 = DotProduct(Normal,PlaneLightVector2);
             for(int i =0;i<pObjectList.size();++i){     //calculate shadow for each sphere
                 if(pObjectList[i]->getflag() < 10 && flag != 96){
                     /*
                      * Test shadow of each sphere
                      * Two light sources; two shadow testing
                      */
                    float NewDiscriminant = shadow(tmp,pObjectList[i]->getCenter(),pObjectList[i]->getRadius(),Light);
                    float NewDiscriminant2 = shadow(tmp,pObjectList[i]->getCenter(),pObjectList[i]->getRadius(),Light2);
                    //std::cout<<NewDiscriminant<<std::endl;
                    if(NewDiscriminant > 0){  //intersect to one of the three spheres --> Blocked !

                        /**I DID SET SHADOW TO BLACK BECAUSE IT LOOKS BETTER I THINK**/
                        if(PlaneDiffuseTerm > 0){
                            PixelColour = addcolour(flag,PlaneDiffuseTerm,PixelColour,blackOrWhite);
                        }

                        if(PlaneDiffuseTerm2 > 0){
                            PixelColour = addcolour(flag,PlaneDiffuseTerm2,PixelColour,blackOrWhite);
                        }
                        PixelColour = MultiplyScalar(PixelColour,0.125);//DARK
                        SetColor(shade, PixelColour);
                        return shade;
                    }

                    if(NewDiscriminant2 > 0){  //intersect to one of the three spheres --> Blocked !
                        if(PlaneDiffuseTerm > 0){
                            PixelColour = addcolour(flag,PlaneDiffuseTerm,PixelColour,blackOrWhite);
                        }

                        if(PlaneDiffuseTerm2 > 0){
                            PixelColour = addcolour(flag,PlaneDiffuseTerm2,PixelColour,blackOrWhite);
                        }
                        PixelColour = MultiplyScalar(PixelColour,0.125);
                        SetColor(shade, PixelColour);
                        return shade;
                    }
                }
             }
             /* plane diffuse shading */
                if(PlaneDiffuseTerm > 0){
                    PixelColour = addcolour(flag,PlaneDiffuseTerm,PixelColour,blackOrWhite);
                }

                if(PlaneDiffuseTerm2 > 0){
                    PixelColour = addcolour(flag,PlaneDiffuseTerm2,PixelColour,blackOrWhite);
                }
                PixelColour = MultiplyScalar(PixelColour,0.5);
                SetColor(shade, PixelColour);

                 return shade;
        }

        float shadow(Vector3 planeintersection,Vector3 SphereCenter,float SphereRadius,Vector3 Light){
            /* same intersection code but from different perspective */
            Vector3 Camera = planeintersection;      //define a new camera point = ray-plane intersection point
            Vector3 NewDirection = Minus(Light,Camera);//ray-plane intersection point to light
            float len1 = sqrt(NewDirection.x*NewDirection.x+NewDirection.y*NewDirection.y+NewDirection.z*NewDirection.z);
            NewDirection = Normalize(NewDirection);
            Vector3 NewOC = Minus(Camera, SphereCenter);
            float len2 = sqrt(NewOC.x*NewOC.x+NewOC.y*NewOC.y+NewOC.z*NewOC.z);
            float NewD_Dot_OC = DotProduct(NewDirection,NewOC);
            float NewOCSqure = DotProduct(NewOC,NewOC);
            if(len2 > len1){
                return -1;
            }
            return (SphereRadius*SphereRadius) - NewOCSqure + NewD_Dot_OC * NewD_Dot_OC;

        }
};
