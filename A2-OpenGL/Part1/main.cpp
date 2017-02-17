#include "Image/Canvas.h"
#include <math.h>

unsigned int width = 512;
unsigned int height = 512;
double length = 0.5;
double scale = 0.1;//need 3d calculation

float vppos_x = 0;
float vppos_y = 0;//1 for the top -1 for the bottom
float setX = 0;
float setY = 0;
float distance = -1.2;
//fprintf(stderr, "%9.6f\n", vppos_y);
bool leftButtonPressed = false;
bool rightButtonPressed = false;
float linelength = 0;
int timercount = 0;
bool rotateCW = true; //Whether we're rotating in the clockwise direction
float rotateAngle = 0; //The angle the square currently rotated
float rotateSpeed = 0.05; //The speed of the rotation
Canvas canvas;

struct Vector4
{
	float x, y, z, w;
	Vector4(){
		x = y = z = 0;
		w = 1;
	}
	Vector4(float x_in, float y_in, float z_in, float w_in)
	{
		x = x_in;
		y = y_in;
		z = z_in;
		w = w_in;
	}

};
Vector4 CrossProduct(Vector4 a, Vector4 b) { 
	return Vector4( a.y*b.z-a.z*b.y,
					a.z*b.x-a.x*b.z,
					a.x*b.y-a.y*b.x,
					1);
}

Vector4 Minus(Vector4 a, Vector4 b) { return Vector4(a.x - b.x, a.y - b.y, a.z - b.z, 1); }
Vector4 Normalize(Vector4 a)
{
	float length = a.x * a.x + a.y * a.y + a.z * a.z;
	length = sqrt(length);
	return Vector4(a.x / length, a.y / length, a.z / length, 1);
}
float Length(Vector4 a, Vector4 b){
	float length = a.x * a.x + a.y * a.y + a.z * a.z;
	length = sqrt(length);
	return length;
}
Vector4 Reverse(Vector4 a){
	return Vector4(-a.x, -a.y, -a.z, 1);
}
struct Matrix{
	float a, b, c, d,
		e, f, g, h,
		i, j, k, l, 
		m, n, o, p;
	Matrix(float a_in, float b_in, float c_in, float d_in,
		float e_in, float f_in, float g_in, float h_in,
		float i_in, float j_in, float k_in, float l_in,
		float m_in, float n_in, float o_in, float p_in){

		a = a_in; b = b_in; c = c_in; d = d_in;
		e = e_in; f = f_in; g = g_in; h = h_in;
		i = i_in; j = j_in; k = k_in; l = l_in;
		m = m_in; n = n_in; o = o_in; p = p_in;
	}
};
Matrix multiply(Matrix ma, Matrix mb){
	Matrix m = Matrix(ma.a*mb.a + ma.b*mb.e + ma.c*mb.i + ma.d*mb.m,
					  ma.a*mb.b + ma.b*mb.f + ma.c*mb.j + ma.d*mb.n,
					  ma.a*mb.c + ma.b*mb.g + ma.c*mb.k + ma.d*mb.o,
					  ma.a*mb.d + ma.b*mb.h + ma.c*mb.l + ma.d*mb.p,

					  ma.e*mb.a + ma.f*mb.e + ma.g*mb.i + ma.h*mb.m,
					  ma.e*mb.b + ma.f*mb.f + ma.g*mb.j + ma.h*mb.n,
					  ma.e*mb.c + ma.f*mb.g + ma.g*mb.k + ma.h*mb.o,
					  ma.e*mb.d + ma.f*mb.h + ma.g*mb.l + ma.h*mb.p,

					  ma.i*mb.a + ma.j*mb.e + ma.k*mb.i + ma.l*mb.m,
					  ma.i*mb.b + ma.j*mb.f + ma.k*mb.j + ma.l*mb.n,
					  ma.i*mb.c + ma.j*mb.g + ma.k*mb.k + ma.l*mb.o,
					  ma.i*mb.d + ma.j*mb.h + ma.k*mb.l + ma.l*mb.p,

					  ma.m*mb.a + ma.n*mb.e + ma.o*mb.i + ma.p*mb.m,
					  ma.m*mb.b + ma.n*mb.f + ma.o*mb.j + ma.p*mb.n,
					  ma.m*mb.c + ma.n*mb.g + ma.o*mb.k + ma.p*mb.o,
					  ma.m*mb.d + ma.n*mb.h + ma.o*mb.l + ma.p*mb.p);
	return m;
};
std::vector<Vector4> vecBuffer;
Vector4 camera;
Vector4 origin;

void SetCamera(){
	camera = Vector4(0, 0, distance, 1);
	origin = Vector4(0, 0, 0, 1);
}
void MouseMove(double x, double y)
{
   //the pointer has moved
   vppos_x = (float)(x) / 256 - 1;
   vppos_y = 1 - (float)(y) / 256;
}

void MouseButton(MouseButtons mouseButton, bool press)
{
    //What to do with the keys?
    if (mouseButton == LeftButton)
    {
		if (press == true) {
			leftButtonPressed = true;
			setX += vppos_x;
			setY += vppos_y;
		}
		else {
			leftButtonPressed = false;
			setX -= vppos_x;
			setY -= vppos_y;
		}
	}
	else if (mouseButton == RightButton){
		if (press == true) {
			rightButtonPressed = true;
		}
		else{
			rightButtonPressed = false;
		}
	}
}

void KeyPress(char keychar)
{
    //A key is pressed! print a message
	if (keychar == ' ') rotateCW = !rotateCW;
    fprintf(stderr, "The \"%c\" key is pressed!\n", keychar);
}

void OnPaint()
{
    canvas.Clear();
	float l = distance;
	if (leftButtonPressed == true){
		float cx = vppos_x + setX;
		float cy = vppos_y + setY;

		camera.x = -l*sin(vppos_x + setX)*cos(vppos_y + setY);
		camera.y = l*sin(vppos_y + setY);
		camera.z = l*cos(vppos_y + setY) *cos(vppos_x + setX);
		//fprintf(stderr, "%.1f, %.1f, %.1f\n", camera.x, camera.y, camera.z);
	}
	if (rightButtonPressed == true){
		distance = (vppos_x + setX);
		camera.z = distance;
	}

	Vector4 gaze = Minus(origin, camera);
	Vector4 W = Normalize(gaze);
	W = Reverse(W);

	Vector4 t = Vector4(0, 1, 0, 1);
	Vector4 U = CrossProduct(t, W);
	U = Normalize(U);

	Vector4 V = CrossProduct(W, U);
	V = Normalize(V);

	Matrix morth = Matrix(2, 0, 0, 0,
						  0, 2, 0, 0,
						  0, 0, 2, 0,
						  0, 0, 0, 1);

	Matrix mvp = Matrix(1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1);

	//Matrix m0 = multiply(mvp, morth);
	

	Matrix mvort = Matrix(U.x, U.y, U.z, 0,
						  V.x, V.y, V.z, 0,
						  W.x, W.y, W.z, 0,
						  0,   0,   0,   1);
	
	Matrix me = Matrix(1, 0, 0, -camera.x,
					   0, 1, 0, -camera.y,
					   0, 0, 1, -camera.z,
					   0, 0, 0, 1);
						
	Matrix mv = multiply(mvort, me);
	
	Matrix mp = Matrix(1, 0, 0, 0,
					   0, 1, 0, 0,
					   0, 0, 0, 0.5,
					   0, 0, -2, 0);

	Matrix m = multiply(mp, multiply(morth, mv));
	//Matrix m = multiply(mp, mv);

	for (int i = 0; i < 4; ++i)
	{ //rotate the vertices
		Vector4 startPoint;
		float x = vecBuffer[i].x;
		float y = vecBuffer[i].y;
		float z = vecBuffer[i].z;
		float w = vecBuffer[i].w;
		startPoint.x = m.a*x + m.b*y + m.c*z + m.d*w;
		startPoint.y = m.e*x + m.f*y + m.g*z + m.h*w;
		startPoint.z = m.i*x + m.j*y + m.k*z + m.l*w;
		startPoint.w = m.m*x + m.n*y + m.o*z + m.p*w;
		
		Vector4 endPoint;
		x = vecBuffer[i+1].x;
		y = vecBuffer[i+1].y;
		z = vecBuffer[i+1].z;
		w = vecBuffer[i+1].w;
		endPoint.x = m.a*x + m.b*y + m.c*z + m.d*w;
		endPoint.y = m.e*x + m.f*y + m.g*z + m.h*w;
		endPoint.z = m.i*x + m.j*y + m.k*z + m.l*w;
		endPoint.w = m.m*x + m.n*y + m.o*z + m.p*w;

		canvas.AddLine(startPoint.x / startPoint.w, startPoint.y / startPoint.w,
			endPoint.x / endPoint.w, endPoint.y / endPoint.w);
	}
	for (int i = 5; i < 9; ++i)
	{ //rotate the vertices
		Vector4 startPoint;
		float x = vecBuffer[i].x;
		float y = vecBuffer[i].y;
		float z = vecBuffer[i].z;
		float w = vecBuffer[i].w;
		startPoint.x = m.a*x + m.b*y + m.c*z + m.d*w;
		startPoint.y = m.e*x + m.f*y + m.g*z + m.h*w;
		startPoint.z = m.i*x + m.j*y + m.k*z + m.l*w;
		startPoint.w = m.m*x + m.n*y + m.o*z + m.p*w;

		Vector4 endPoint;
		x = vecBuffer[i + 1].x;
		y = vecBuffer[i + 1].y;
		z = vecBuffer[i + 1].z;
		w = vecBuffer[i + 1].w;
		endPoint.x = m.a*x + m.b*y + m.c*z + m.d*w;
		endPoint.y = m.e*x + m.f*y + m.g*z + m.h*w;
		endPoint.z = m.i*x + m.j*y + m.k*z + m.l*w;
		endPoint.w = m.m*x + m.n*y + m.o*z + m.p*w;

		canvas.AddLine(startPoint.x / startPoint.w, startPoint.y / startPoint.w,
			endPoint.x / endPoint.w, endPoint.y / endPoint.w);
	}
	for (int i = 10; i < 18; i+=2)
	{ //rotate the vertices
		Vector4 startPoint;
		float x = vecBuffer[i].x;
		float y = vecBuffer[i].y;
		float z = vecBuffer[i].z;
		float w = vecBuffer[i].w;
		startPoint.x = m.a*x + m.b*y + m.c*z + m.d*w;
		startPoint.y = m.e*x + m.f*y + m.g*z + m.h*w;
		startPoint.z = m.i*x + m.j*y + m.k*z + m.l*w;
		startPoint.w = m.m*x + m.n*y + m.o*z + m.p*w;

		Vector4 endPoint;
		x = vecBuffer[i + 1].x;
		y = vecBuffer[i + 1].y;
		z = vecBuffer[i + 1].z;
		w = vecBuffer[i + 1].w;
		endPoint.x = m.a*x + m.b*y + m.c*z + m.d*w;
		endPoint.y = m.e*x + m.f*y + m.g*z + m.h*w;
		endPoint.z = m.i*x + m.j*y + m.k*z + m.l*w;
		endPoint.w = m.m*x + m.n*y + m.o*z + m.p*w;

		canvas.AddLine(startPoint.x / startPoint.w, startPoint.y / startPoint.w,
			endPoint.x / endPoint.w, endPoint.y / endPoint.w);
	}
}

void OnTimer()
{
	if (rotateCW) rotateAngle -= rotateSpeed;
	else rotateAngle += rotateSpeed;
	linelength = (float)(sin(timercount / 10.0) * 0.1 + 0.1);
	timercount++;
}

int main(int, char **){
	vecBuffer.push_back(Vector4(length, length,-length,1));
	vecBuffer.push_back(Vector4(length, -length, -length,1));
	vecBuffer.push_back(Vector4(-length, -length, -length,1));
	vecBuffer.push_back(Vector4(-length, length, -length,1));
	vecBuffer.push_back(Vector4(length, length, -length,1));

	vecBuffer.push_back(Vector4(length, length, length,1));
	vecBuffer.push_back(Vector4(length, -length, length,1));
	vecBuffer.push_back(Vector4(-length, -length, length,1));
	vecBuffer.push_back(Vector4(-length, length, length,1));
	vecBuffer.push_back(Vector4(length, length, length,1));

	vecBuffer.push_back(Vector4(length, length, -length,1));
	vecBuffer.push_back(Vector4(length, length, length,1));
	vecBuffer.push_back(Vector4(length, -length, -length,1));
	vecBuffer.push_back(Vector4(length, -length, length,1));
	vecBuffer.push_back(Vector4(-length, -length, -length,1));
	vecBuffer.push_back(Vector4(-length, -length, length,1));
	vecBuffer.push_back(Vector4(-length, length, -length,1));
	vecBuffer.push_back(Vector4(-length, length, length,1));
	
	SetCamera();

	
	//fprintf(stderr, "%.1f, %.1f, %.1f, %.1f\n", W.x, W.y, W.z, W.w);
	//fprintf(stderr, "%.1f, %.1f, %.1f, %.1f\n", gaze.x, gaze.y, gaze.z, gaze.w);
	//Vector4 temp = CrossProduct(Vector4(2,3,4,1),Vector4(5,6,7,1));
    //Link the call backs
    canvas.SetMouseMove(MouseMove);
    canvas.SetMouseButton(MouseButton);
    canvas.SetKeyPress(KeyPress);
    canvas.SetOnPaint(OnPaint);
    canvas.SetTimer(0.1, OnTimer);//trigger OnTimer every 0.1 second
    //Show Window
    canvas.Show(width, height, "Canvas Demo");
    return 0;
}
/*Matrix m = multiply(mp, mvort);
Matrix cam = Matrix(cos(vppos_y + setY), 0, sin(vppos_y + setY), 0,
	0, cos(vppos_x + setX), -sin(vppos_x + setX), 0,
	-sin(vppos_y + setY), sin(vppos_x + setX), cos(vppos_x + setX), 0,
	0, 0, 0, 1);

camera.x = cam.a*camera.x + cam.b*camera.y + cam.c*camera.z;
camera.y = cam.e*camera.x + cam.f*camera.y + cam.g*camera.z;
camera.z = cam.i*camera.x + cam.j*camera.y + cam.k*camera.z;*/

/*
if (cx > cy){
camera.x = l*sin(vppos_x + setX);
camera.y = camera.y;
camera.z = l*cos(vppos_x + setX);
}
else{
camera.x = camera.x;
camera.y = l*sin(vppos_y + setY);
camera.z = l*cos(vppos_y + setY);

}*/