//This is a simple ray tracing example showing how to use the Image 
#pragma once
#include "Image.h"
#include <vector>
#include "sphere.h"
#include "cube.h"
#include "mirrorsphere.h"
#include <iostream>
#include "triangle.h"
#include "transphere.h"
float width = 800;
float height = 800;
float depth = 800;

std::vector<Object *> pObjectList;
void pushCube(Cube c){
	pObjectList.push_back(&(c.getTriangles(9)));
}
void RayTrace(Image * pImage)
{
	/*
	 * flags
	 *
	 * 1 = sphere;
	 * 2 = mirror sphere
	 * 3 = transparent sphere
	 * for inside cube view			for outside cube view
	 * 10 = bottom floor			20 = bottom floor
	 * 11 = left wall				10 = bottom floor
	 * 12 = back wall				10 = bottom floor
	 * 13 = right wall				10 = bottom floor
	 * 14 = front wall				10 = bottom floor
	 * 25 = top wall
	 */

	/*****************Define all the objects************
	
	mirrorsphere sphereRefl(Vector3(width / 2, 200, 700), //center
		200, 2);//radius
	Sphere sphere2(Vector3(width / 2 + 200, 75, 200), //center
		75, 1);//radius*/
	

	Cube cube1(Vector3(100, 100, 700), 200, 10);
	Sphere sphere1(Vector3(100, 100+175, 700), 75, 1);
	mirrorsphere sphere2(MirrorShpere, 150, 2);
	transphere sphere3(TransparentSphere, 150, 3);
	pObjectList.push_back(&sphere1);
	pObjectList.push_back(&sphere2);
	pObjectList.push_back(&sphere3);

	pObjectList.push_back(&(cube1.getTriangles(0)));
	pObjectList.push_back(&(cube1.getTriangles(1)));
	pObjectList.push_back(&(cube1.getTriangles(2)));
	pObjectList.push_back(&(cube1.getTriangles(3)));
	pObjectList.push_back(&(cube1.getTriangles(4)));
	pObjectList.push_back(&(cube1.getTriangles(5)));
	pObjectList.push_back(&(cube1.getTriangles(6)));
	pObjectList.push_back(&(cube1.getTriangles(7)));
	pObjectList.push_back(&(cube1.getTriangles(8)));
	pObjectList.push_back(&(cube1.getTriangles(9)));

	triangle BotLeft(Vector3(0, 0, 0), Vector3(0, 0, depth), Vector3(width, 0, depth), 10);
	triangle BotRight(Vector3(0, 0, 0), Vector3(width, 0, 0), Vector3(width, 0, depth), 10);
	// Left plane
	triangle LeftLeft(Vector3(0, 0, depth), Vector3(0, height, depth), Vector3(0, height, 0), 11);
	triangle LeftRight(Vector3(0, 0, depth), Vector3(0, 0, 0), Vector3(0, height, 0), 11);
	// Back plane
	triangle BackLeft(Vector3(0, 0, depth), Vector3(0, height, depth), Vector3(width, height, depth), 12);
	triangle BackRight(Vector3(0, 0, depth), Vector3(width, 0, depth), Vector3(width, height, depth), 12);
	// right plane
	triangle RightLeft(Vector3(width, 0, depth), Vector3(width, height, depth), Vector3(width, height, 0), 13);
	triangle RightRight(Vector3(width, 0, depth), Vector3(width, 0, 0), Vector3(width, height, 0), 13);
	// Top plane
	triangle TopLeft(Vector3(0, height, 0), Vector3(0, height, depth), Vector3(width, height, depth), 15);
	triangle TopRight(Vector3(0, height, 0), Vector3(width, height, 0), Vector3(width, height, depth), 15);

	pObjectList.push_back(&BotLeft);
	pObjectList.push_back(&BotRight);
	pObjectList.push_back(&LeftLeft);
	pObjectList.push_back(&LeftRight);
	pObjectList.push_back(&BackLeft);
	pObjectList.push_back(&BackRight);
	pObjectList.push_back(&RightLeft);
	pObjectList.push_back(&RightRight);
	pObjectList.push_back(&TopLeft);
	pObjectList.push_back(&TopRight);

    /******Set Pixel one by one**********/
    for (int i = 0; i < width-1; ++ i)
        for (int j = 0; j < height-1; ++j){
			
			Vector3 PixelPosition((float)i, (float)j, 0);
		
				//Set up the ray we're tracing: R = O + tD;
				Pixel px;
				
				Vector3 Direction = Minus(PixelPosition, Camera);
				Direction = Normalize(Direction);   //direction of incident ray

				float t_min = 999999;
				Vector3 Normal_min;
				bool HasIntersection = false;
				int index = 0;
				//Intersect with the list of objects
				for (int k = 0; k < pObjectList.size(); ++k)
				{
					float t;
					Vector3 normal;
					/* if intersect ?*/
					bool DoesIntersect = pObjectList[k]->Intersect(Camera, Direction,
						&t, &normal);
					if (DoesIntersect)
					{
						HasIntersection = true;
						if (t_min > t)
						{
							index = k;   //record distance
							t_min = t;
							Normal_min = normal;    //record normal verctor of intersection point
						}
					}
				}

				if (HasIntersection)
				{

					Vector3 Intersection = MultiplyScalar(Direction, t_min);
					Intersection = Add(Intersection, Camera); //intersection point
					/* shading part */
					px = pObjectList[index]->DiffuseShade(pObjectList[index]->getflag(), Direction, Intersection, Normal_min, pObjectList);
				}//if t > 0
				else //No Intersection, set background colour
				{
					SetColor(px, BackgroundColor);
				}
            (*pImage)(i, 800-j) = px;  //change origin
			//(*pImage)(i, 800-j) = average;
		}

}

/*normal sampling
for (int i = 0; i < width - 1; ++i){
for (int j = 0; j < height - 1; ++j){
Pixel px1 = (*pImage)(i, j);
Pixel px2 = (*pImage)(i, j + 1);
Pixel px3 = (*pImage)(i + 1, j);
Pixel px4 = (*pImage)(i + 1, j + 1);

Pixel px;
px.R = (px1.R + px2.R + px3.R + px4.R) / 4;
px.G = (px1.G + px2.G + px3.G + px4.G) / 4;
px.B = (px1.B + px2.B + px3.B + px4.B) / 4;
(*pImage)(i, j) = px;
}
}*/
