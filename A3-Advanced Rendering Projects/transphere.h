#pragma once
#include <math.h>
#include <ctgmath>
#include <cmath>
#include "object.h"
#include "common.h"
#include <iostream>

class transphere : public Object
{
	Vector3 Center;
	float Radius;
	int flag;

public:
	transphere(Vector3 Center_in, float Radius_in, int flag_in)
	{
		Center = Center_in;
		Radius = Radius_in;
		flag = flag_in;
	}
	virtual Vector3 getCenter(){ return Center; }
	virtual float getRadius(){ return Radius; }
	virtual int getflag() { return flag; }

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
		}
		else
		{
			return false;
		}
	}

	virtual Pixel DiffuseShade(int flag, Vector3 Direction, Vector3 Intersection, Vector3 Normal, std::vector<Object *> pObjectList)
	{
		float miu = 1;
		float miut = 1.5;

		Pixel shade;
		int index = 0;
		bool HasIntersection = false;
		float t_min = 999999;
		Vector3 Normal_min;

		Vector3 d = Direction;
		Vector3 n = Normal;
		
		//caluculate transparent //t = (u / ut)(d - n(d.n))¨C n 1 - n 2(1 - (d.n)2)
		float a = DotProduct(d, n);
		float b = 1 - a * a;
		float c = DotProduct(n, n) * b;
		float e = (1 - c) / (miut*miut);
		e = sqrt(e);
		Vector3 temp1 = MultiplyScalar(n,-e);
		float f = DotProduct(d , n);
		Vector3 temp2 = MultiplyScalar(n, -f);
		temp2 = Add(d, temp2);
		temp2 = MultiplyScalar(temp2, (miu/miut));
		Vector3 t = Add(temp2, temp1);
		Vector3 r = Normalize(t); 

		Vector3 l = Minus(TransparentSphere, Intersection);
		float scale = DotProduct(r, l);
		Vector3 outIntersection = MultiplyScalar(r, scale * 2);
		outIntersection = Add(Intersection, outIntersection);


		a = DotProduct(t, n);
		b = 1 - a*a;
		c = DotProduct(n, n)*b;
		e = (1 - c)/(miu*miu);
		e = sqrt(e);
		temp1 = MultiplyScalar(n,-e);
		f = DotProduct(t, n);
		temp2 = MultiplyScalar(n, -f);
		temp2 = Add(t, temp2);
		temp2 = MultiplyScalar(temp2, (miut/miu));
		t = Add(temp2, temp1);
		r = Normalize(t);

		


		for (int i = 0; i<pObjectList.size(); ++i){
			if (pObjectList[i]->getflag() != 3){
				float t2;
				Vector3 normal2;
				bool DoesIntersect = pObjectList[i]->Intersect(outIntersection, r,
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
			intersection = Add(intersection, outIntersection);
			shade = pObjectList[index]->DiffuseShade(pObjectList[index]->getflag(), r, intersection, Normal_min, pObjectList);
		}
		else //No Intersection, set background colour
		{
			SetColor(shade, BackgroundColor);
		}
		return shade;
	}


};