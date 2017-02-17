#pragma once
#include "object.h"
#include "common.h"
#include "triangle.h"
#include <iostream>

class Cube : public Object
{
	/* Three Vertex of a triangle */
	Vector3 Center; //center
	float l;		//length
	int flag;       //flag
	triangle TriArray[12];
public:
	Cube(Vector3 Center_in, float l_in, int flag_in)
	{
		Center = Center_in;
		l = l_in/2;
		flag = flag_in;

		float x = Center.x;
		float y = Center.y;
		float z = Center.z;

		TriArray[0] = triangle(Vector3(x - l, y - l, z - l), Vector3(x - l, y - l, z + l), Vector3(x + l, y - l, z + l), 20);
		TriArray[1] = triangle(Vector3(x - l, y - l, z - l), Vector3(x + l, y - l, z - l), Vector3(x + l, y - l, z + l), 20);

		TriArray[2] = triangle(Vector3(x - l, y - l, z - l), Vector3(x - l, y + l, z - l), Vector3(x - l, y + l, z + l), 21);
		TriArray[3] = triangle(Vector3(x - l, y - l, z - l), Vector3(x - l, y - l, z + l), Vector3(x - l, y + l, z + l), 21);

		TriArray[4] = triangle(Vector3(x - l, y - l, z + l), Vector3(x - l, y + l, z + l), Vector3(x + l, y + l, z + l), 22);
		TriArray[5] = triangle(Vector3(x - l, y - l, z + l), Vector3(x + l, y - l, z + l), Vector3(x + l, y + l, z + l), 22);

		TriArray[6] = triangle(Vector3(x + l, y - l, z - l), Vector3(x + l, y + l, z - l), Vector3(x + l, y + l, z + l), 23);
		TriArray[7] = triangle(Vector3(x + l, y - l, z - l), Vector3(x + l, y - l, z + l), Vector3(x + l, y + l, z + l), 23);

		TriArray[8] = triangle(Vector3(x - l, y - l, z - l), Vector3(x - l, y + l, z - l), Vector3(x + l, y + l, z - l), 24);
		TriArray[9] = triangle(Vector3(x - l, y - l, z - l), Vector3(x + l, y - l, z - l), Vector3(x + l, y + l, z - l), 24);

		TriArray[0] = triangle(Vector3(x - l, y + l, z - l), Vector3(x - l, y + l, z + l), Vector3(x + l, y + l, z + l), 25);
		TriArray[1] = triangle(Vector3(x - l, y + l, z - l), Vector3(x + l, y + l, z - l), Vector3(x + l, y + l, z + l), 25);
		
	}
	virtual Vector3 getCenter(){ return Center; }
	virtual float getRadius(){ return l; }
	virtual int getflag() { return flag; }
	triangle getTriangles(int i){
		return TriArray[i];
	}

	virtual bool Intersect(Vector3 Origin, Vector3 Direction,
		float *t, Vector3 *normal)
	{
		return true;
	}

	Vector3 addcolour(int flag, float PlaneDiffuseTerm, Vector3 PixelColour, int blackOrWhite){
		return PixelColour;
	}

	virtual Pixel DiffuseShade(int flag, Vector3 Direction, Vector3 Surface, Vector3 Normal, std::vector<Object *> pObjectList)
	{
		Pixel shade;
		Vector3 PixelColour = AmbientColour;
		/* Checker board */
		int blackOrWhite = -1;
		float result = abs((int)Surface.x / 50 % 2);
		float result1 = abs((int)Surface.z / 50 % 2);
		//std::cout << result << std::endl;
		if (result == result1){
			blackOrWhite = 1;
		}
		if (Surface.x < 0){
			blackOrWhite = blackOrWhite * -1;
		}
		if (Surface.z < 0){
			blackOrWhite = blackOrWhite * -1;
		}
		/********************/
		Vector3 PlaneLightVector = Minus(Light, Surface); //L
		Vector3 PlaneLightVector2 = Minus(Light2, Surface); //L
		PlaneLightVector = Normalize(PlaneLightVector);
		PlaneLightVector2 = Normalize(PlaneLightVector2);
		Vector3 tmp = Surface;
		Surface = Normalize(Surface);
		float PlaneDiffuseTerm = DotProduct(Normal, PlaneLightVector);
		float PlaneDiffuseTerm2 = DotProduct(Normal, PlaneLightVector2);
		for (int i = 0; i<pObjectList.size(); ++i){     //calculate shadow for each sphere
			if (pObjectList[i]->getflag() < 10 && flag != 96){
				/*
				* Test shadow of each sphere
				* Two light sources; two shadow testing
				*/
				float NewDiscriminant = shadow(tmp, pObjectList[i]->getCenter(), pObjectList[i]->getRadius(), Light);
				float NewDiscriminant2 = shadow(tmp, pObjectList[i]->getCenter(), pObjectList[i]->getRadius(), Light2);
				//std::cout<<NewDiscriminant<<std::endl;
				if (NewDiscriminant > 0){  //intersect to one of the three spheres --> Blocked !

					/**I DID SET SHADOW TO BLACK BECAUSE IT LOOKS BETTER I THINK**/
					if (PlaneDiffuseTerm > 0){
						PixelColour = addcolour(flag, PlaneDiffuseTerm, PixelColour, blackOrWhite);
					}

					if (PlaneDiffuseTerm2 > 0){
						PixelColour = addcolour(flag, PlaneDiffuseTerm2, PixelColour, blackOrWhite);
					}
					PixelColour = MultiplyScalar(PixelColour, 0.125);//DARK
					SetColor(shade, PixelColour);
					return shade;
				}

				if (NewDiscriminant2 > 0){  //intersect to one of the three spheres --> Blocked !
					if (PlaneDiffuseTerm > 0){
						PixelColour = addcolour(flag, PlaneDiffuseTerm, PixelColour, blackOrWhite);
					}

					if (PlaneDiffuseTerm2 > 0){
						PixelColour = addcolour(flag, PlaneDiffuseTerm2, PixelColour, blackOrWhite);
					}
					PixelColour = MultiplyScalar(PixelColour, 0.125);
					SetColor(shade, PixelColour);
					return shade;
				}
			}
		}
		/* plane diffuse shading */
		if (PlaneDiffuseTerm > 0){
			PixelColour = addcolour(flag, PlaneDiffuseTerm, PixelColour, blackOrWhite);
		}

		if (PlaneDiffuseTerm2 > 0){
			PixelColour = addcolour(flag, PlaneDiffuseTerm2, PixelColour, blackOrWhite);
		}
		PixelColour = MultiplyScalar(PixelColour, 0.5);
		SetColor(shade, PixelColour);

		return shade;
	}

	float shadow(Vector3 planeintersection, Vector3 SphereCenter, float SphereRadius, Vector3 Light){
		/* same intersection code but from different perspective */
		Vector3 Camera = planeintersection;      //define a new camera point = ray-plane intersection point
		Vector3 NewDirection = Minus(Light, Camera);//ray-plane intersection point to light
		float len1 = sqrt(NewDirection.x*NewDirection.x + NewDirection.y*NewDirection.y + NewDirection.z*NewDirection.z);
		NewDirection = Normalize(NewDirection);
		Vector3 NewOC = Minus(Camera, SphereCenter);
		float len2 = sqrt(NewOC.x*NewOC.x + NewOC.y*NewOC.y + NewOC.z*NewOC.z);
		float NewD_Dot_OC = DotProduct(NewDirection, NewOC);
		float NewOCSqure = DotProduct(NewOC, NewOC);
		if (len2 > len1){
			return -1;
		}
		return (SphereRadius*SphereRadius) - NewOCSqure + NewD_Dot_OC * NewD_Dot_OC;

	}
};
