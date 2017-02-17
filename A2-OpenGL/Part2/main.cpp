#include "Canvas.h"
#include <math.h>

unsigned int width = 600;
unsigned int height = 600;
float vppos_x = 0;
float vppos_y = 0;//1 for the top -1 for the bottom
float setX = 0;
float setY = 0;
bool leftButtonPressed = false;
bool rightButtonPressed = false;
float Rotation = 0;
float RotatingSpeed = 0.02;
float distance = -1.2;
float camera[3];
float origin[3] = { 0, 0, 0 };
float CameraPos[3];
float SpecularColor[3];
float light[3] = { 0, 0, -5 };
Canvas canvas;

const GLfloat vpoint[] = {
	-.5f, -.5f, -0.5f, //near
	0.5f, -.5f, -0.5f,
	-.5f, 0.5f, -0.5f, //upper half of the square
	0.5f, -.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
	-.5f, 0.5f, -0.5f, //lower half of the squre

	-.5f, -.5f, 0.5f, //far
	0.5f, -.5f, 0.5f,
	-.5f, 0.5f, 0.5f, //upper half of the square
	0.5f, -.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	-.5f, 0.5f, 0.5f, //lower half of the squre

	-.5f, -.5f, -.5f, //left
	-.5f, -.5f, 0.5f,
	-.5f, 0.5f, -.5f,
	-.5f, -.5f, 0.5f,
	-.5f, 0.5f, 0.5f,
	-.5f, 0.5f, -.5f,

	0.5f, -.5f, -.5f, //right
	0.5f, -.5f, 0.5f,
	0.5f, 0.5f, -.5f,
	0.5f, -.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, -.5f,

	-.5f, -.5f, 0.5f, //top
	0.5f, -.5f, 0.5f,
	-.5f, -.5f, -.5f,
	0.5f, -.5f, 0.5f,
	0.5f, -.5f, -.5f,
	-.5f, -.5f, -.5f,

	-.5f, 0.5f, 0.5f, //bot
	0.5f, 0.5f, 0.5f,
	-.5f, 0.5f, -.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, -.5f,
	-.5f, 0.5f, -.5f
};
const GLfloat normals[] = {
	0, 0, -1,
	0, 0, -1,
	0, 0, -1,
	0, 0, -1,
	0, 0, -1,
	0, 0, -1,

	0, 0, 1,
	0, 0, 1,
	0, 0, 1,
	0, 0, 1,
	0, 0, 1,
	0, 0, 1,  //six vertices have the same normal!

	-1, 0, 0,
	-1, 0, 0,
	-1, 0, 0,
	-1, 0, 0,
	-1, 0, 0,
	-1, 0, 0,

	1, 0, 0,
	1, 0, 0,
	1, 0, 0,
	1, 0, 0,
	1, 0, 0,
	1, 0, 0,

	0, -1, 0,
	0, -1, 0,
	0, -1, 0,
	0, -1, 0,
	0, -1, 0,
	0, -1, 0,

	0, 1, 0,
	0, 1, 0,
	0, 1, 0,
	0, 1, 0,
	0, 1, 0,
	0, 1, 0
};
const GLfloat vtexcoord[] = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f, //upper half of the square
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f 
}; //lower half of the square

const char * vshader_square = " \
	#version 330 core \n\
	in vec3 vpoint; \
	uniform float rotation; \
	uniform vec3 cameraPos; \
	in vec2 vtexcoord; \
	in vec3 normal_in; \
	out vec3 normal_transformed; \
	out vec3 object_pos; \
	out vec2 uv; \
							\
	mat4 mo(){\
		mat4 morth = mat4(1); \
		morth[3][3] = 1;\
		return morth;\
	}\
	mat4 mv(){\
		vec3 W = normalize(cameraPos);\
		vec3 t = vec3(0, 1,0);\
		\
		vec3 U = normalize(cross(t, W));\
		\
		vec3 V = normalize(cross(W, U));\
		\
		vec3 column0 = vec3(U.x, U.y, U.z);\
		vec3 column1 = vec3(V.x, V.y, V.z);\
		vec3 column2 = vec3(W.x, W.y, W.z);\
		mat3 temp = mat3(column0, column1, column2);\
		mat4 mvort = mat4(temp);\
		\
		mat4 meye = mat4(1);\
		meye[0][3] = -cameraPos.x;\
		meye[1][3] = -cameraPos.y;\
		meye[2][3] = -cameraPos.z;\
		return mvort; \
	}\
	mat4 mp(){\
		mat4 mper = mat4(1); \
		mper[2][2] =  0;\
		mper[2][3] =  2;\
		mper[3][2] =  -0.5;\
		mper[3][3] =  2;\
		return mper; \
	}\
	\
	void main() { \
		uv = vtexcoord; \
		normal_transformed = normal_in; \
		gl_Position =mp()*mv() *mo()* vec4(vpoint, 1); \
		object_pos = gl_Position.xyz; \
	} \
																																												  																																												          ";

const char * fshader_square = " \
	#version 330 core \n\
	in vec2 uv; \
	in vec3 object_pos; \
	in vec3 normal_transformed; \
	out vec3 color; \
	uniform sampler2D tex;\
	uniform vec3 light_pos; \
	uniform vec3 camera_pos; \
	uniform vec3 specular_color;\
	void main() { \
		vec3 diffuse_color = texture(tex, uv).rgb; \
			vec3 ambient_color = diffuse_color * 0.1;\
			vec3 light_vec = light_pos - object_pos;\
			light_vec = normalize(light_vec);\
			float ndotl = dot(light_vec, normal_transformed);\
			if (ndotl > 0) {\
				diffuse_color = diffuse_color * ndotl; \
				vec3 eye_vec = camera_pos - object_pos; \
				eye_vec = normalize(eye_vec);\
				vec3 h = eye_vec + light_vec; \
				h = normalize(h); \
				float hdotn = dot(h, normal_transformed); \
				if (hdotn < 0) hdotn = 0; \
				float specular_term = pow(hdotn, 100);\
				color = ambient_color + diffuse_color + specular_color * specular_term; \
			}\
			else color = ambient_color; \
	}";

//OpenGL context variables
GLuint programID = 0;
GLuint VertexArrayID = 0;
GLuint RotBindingID = 0;
GLuint LightPosID = 0;
GLuint CamID = 0;

void setCamera(){
	camera[0] = 0;
	camera[1] = 0;
	camera[2] = distance;
}
void InitializeGL()
{
	//Compile the shaders
	programID = compile_shaders(vshader_square, fshader_square);

	//Generate Vertex Array and bind the vertex buffer data
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	///--- Generate memory for vertexbuffer
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	/// The subsequent commands will affect the specified buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	/// Pass the vertex positions to OpenGL
	glBufferData(GL_ARRAY_BUFFER, sizeof(vpoint), vpoint, GL_STATIC_DRAW);

	///--- find the binding point in the shader:
	/// "vpoint" in the vertex shader
	glUseProgram(programID);
	GLuint vpoint_id = glGetAttribLocation(programID, "vpoint");
	glEnableVertexAttribArray(vpoint_id);
	glVertexAttribPointer(vpoint_id,
		3, //size per vertex (3 floats for cord)
		GL_FLOAT,
		false, //don't normalize
		0, //stride = 0
		0); //offset = 0
	//Find the binding point for the uniform variable
	//"rotation" in vertex shader
	GLuint texcoordbuffer;
	glGenBuffers(1, &texcoordbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, texcoordbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtexcoord), vtexcoord, GL_STATIC_DRAW);
	GLuint texcoordBindingPosition = glGetAttribLocation(programID, "vtexcoord");
	glEnableVertexAttribArray(texcoordBindingPosition);
	glVertexAttribPointer(texcoordBindingPosition, 2, GL_FLOAT,
		GL_FALSE, 0, (void *)0);

	/// --- Load the texture image
	Texture teximage = LoadPNGTexture("texture.png");

	/// --- Set openGL texture parameters
	GLuint texobject;
	glGenTextures(1, &texobject);
	glBindTexture(GL_TEXTURE_2D, texobject);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, teximage.width,
		teximage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		teximage.dataptr);

	/// --- Activate the loaded texture in channel 0
	GLuint tex_bindingpoint = glGetUniformLocation(programID, "tex");
	glUniform1i(tex_bindingpoint, 0 /*GL_TEXTURE0*/);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texobject);
	RotBindingID = glGetUniformLocation(programID, "rotation");
	CamID = glGetUniformLocation(programID, "cameraPos");
	GLuint camera_pos_id = glGetUniformLocation(programID, "camera_pos");
	glUniform3f(camera_pos_id, CameraPos[0], CameraPos[1], CameraPos[2]);
	SpecularColor[0] = 1;
	SpecularColor[1] = 1;
	SpecularColor[2] = 1;
	GLuint specular_id = glGetUniformLocation(programID,
		"specular_color");
	glUniform3f(specular_id, SpecularColor[0], SpecularColor[1],
		SpecularColor[2]);
	LightPosID = glGetUniformLocation(programID, "light_pos");
	//Add a global variable LightPosID on top
	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	GLuint normal_id = glGetAttribLocation(programID, "normal_in");
	glEnableVertexAttribArray(normal_id);
	glVertexAttribPointer(normal_id, 3, GL_FLOAT, false, 0, 0);
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

}

void OnPaint()
{
	float l = distance;
	if (leftButtonPressed == true){
		float cx = vppos_x + setX;
		float cy = vppos_y + setY;
		//camera[0] = l*sin(vppos_x + setX);
		//camera[2] = l*cos(vppos_x + setX);
		camera[0]= l*sin(vppos_x + setX)*cos(vppos_y + setY);
		camera[1]= l*sin(vppos_y + setY);
		camera[2]= l*cos(vppos_y + setY) *cos(vppos_x + setX);
		
		//camera[0] = l*sin(vppos_x + setX);
		//camera[1] = -l*sin(vppos_y + setY);
		//camera[2] = l*0.8*cos(vppos_y + setY) *cos(vppos_x + setX);
		//fprintf(stderr, "%.1f, %.1f, %.1f\n", camera.x, camera.y, camera.z);
	}
	if (rightButtonPressed == true){
		distance = (vppos_y + setY);
		camera[2] += distance;
	}
	//Binding the openGL context
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(programID);
	glBindVertexArray(VertexArrayID);
	//upload current rotation to GPU
	glUniform1f(RotBindingID, Rotation);
	glUniform3f(CamID, camera[0], camera[1], camera[2]);
	glUniform3f(LightPosID, light[0], light[1], light[2]);
	glDrawArrays(GL_TRIANGLES, 0 /*buffer offset*/, 6*6 /*#vertices*/);
	//Clean up the openGL context for other drawings
	glUseProgram(0);
	glBindVertexArray(0);
}

void OnTimer()
{
	Rotation += RotatingSpeed;
	light[0] = sin(Rotation);
	light[2] = cos(Rotation);
}

int main(int, char **){
	//Link the call backs
	setCamera();
	canvas.SetMouseMove(MouseMove);
	canvas.SetMouseButton(MouseButton);
	canvas.SetKeyPress(KeyPress);
	canvas.SetOnPaint(OnPaint);
	canvas.SetTimer(0.05, OnTimer);
	//Show Window
	canvas.Initialize(width, height, "Rotating Square Demo");
	//Do our initialization
	InitializeGL();
	canvas.Show();

	return 0;
}

//gl_Position =  perMatrix() * RotationMatrix(rotation) * camMatrix()* vec4(vpoint, 1); \
//	gl_Position =  camMatrix()* perMatrix() * RotationMatrix(rotation)*  vec4(vpoint, 1); \

/*inside
	mat4 mper = mat4(1); \
		mper[2][2] =  0;\
		mper[2][3] =  -2;\
		mper[3][2] =  0.5;\
		mper[3][3] =  0.5;\
		return mper; \
		*/