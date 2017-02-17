//This is a simple ray tracing example showing how to use the Image 
#pragma once
#include "Image.h"
#include <vector>
#include "sphere.h"
#include "floor.h"


#include <iostream>

void RayTraceSphere(Image * pImage)
{
    std::vector<Object *> pObjectList;
    //Floor floor(Vector3(1, 0, 0),Vector3(0, 0, 0), Vector3(0, 1, 0));
    Floor floor(Vector3(1024, -512, 0),Vector3(512, 512, 700), Vector3(1024, 1536, 0));
    Floor leftWall(Vector3(1024, 0, 0),Vector3(512, 512, 1000), Vector3(0, 0, 0));
    Sphere sphere(Vector3(512, 512, 450), 250);//radius//center
    //Sphere sphere2(Vector3(256, 512, 450), 250);//radius//center

    pObjectList.push_back(&floor);
    pObjectList.push_back(&sphere);
    //pObjectList.push_back(&leftWall);

    Vector3 Camera(512, 512, -400);

    for (int i = 0; i < 1024; ++ i)
        for (int j = 0; j < 1024; ++j)
		{
            //Set up the ray we're tracing: R = O + tD;
            Pixel px;
            Vector3 PixelPosition((float)j, (float)i, 0);
			Vector3 Direction = Minus(PixelPosition, Camera);
			Direction = Normalize(Direction);


            float t_min = 999999;
            Vector3 Normal_min;
            bool HasIntersection = false;
            int index;
            //Intersect with the list of objects
            for (int k = 0; k < pObjectList.size(); ++ k)
            {
                float t;
                Vector3 normal;

                bool DoesIntersect = pObjectList[k]->Intersect(Camera, Direction,
                                                             &t, &normal);
                if (DoesIntersect)
                {
                    HasIntersection = true;
                    if (t_min > t)
                    {
                        index = k;
                        t_min = t;
                        Normal_min = normal;
                    }
                }
            }

            if (HasIntersection)
			{              
                Vector3 Intersection = MultiplyScalar(Direction, t_min);
				Intersection = Add(Intersection, Camera);
                px = pObjectList[index]->DiffuseShade(pObjectList[index]->getType(), Camera, Intersection, Normal_min);
			}//if t > 0
			else //No Intersection, set background colour
			{
				SetColor(px, BackgroundColor);
			}
			 
			(*pImage)(i, j) = px;
		}
}
