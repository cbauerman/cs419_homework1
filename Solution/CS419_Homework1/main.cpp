#include <cmath>
#include <vector>
#include "openglutl.h"
#include "SOIL.h"

typedef vec4  color4;
typedef vec4  point4;

#pragma region perspective
//perspective variables

// Projection transformation parameters for ORTHO viewing
GLfloat  left = 0.0, right = 4.0;
GLfloat  bottom = -1.0, top = 3.0;
GLfloat  zoNear = 4, zoFar = 15;

// Projection transformation parameters for PERSPEC viewing
GLfloat fovy = 90.0, aspect = 1.0;
GLfloat zpNear = 0.1, zpFar = 15;

//perspective
#pragma endregion

#pragma region GLuints

// Locations of uniform variables in shader program
GLuint ModelView;
GLuint Projection;
GLuint AmbientProduct;
GLuint DiffuseProduct;
GLuint SpecularProduct;
GLuint LightPosition;
GLuint Shininess;
GLuint Rot;

//program
GLuint program;

//Gluints
#pragma endregion

#pragma region matrices

//Model-view and projection matrices
mat4  mv, proj;

vec4 rot;

//matrices
#pragma endregion 

#pragma region camera

point4  eye = point4(0.0, 0.0, 2.0, 1.0);
point4  at  = point4(0.0, 0.0, 0.0, 1.0);
vec4    up  = vec4  (0.0, 1.0, 0.0, 0.0);

projection projType = PERSPEC;

//camera
#pragma endregion

#pragma region ui elements

bool buttonHeld = false;

bool velocity = false;

int screenWidth  = 512;
int screenHeight = 512;

double xPrev, yPrev;
double xCur, yCur;

//ui elements
#pragma endregion 

#pragma region object properties

#define PLANEAMB vec4(.658824, .658824 , .658824,  1.0)
#define PLANEDIF vec4(.658824, .658824 , .658824,  1.0)
#define PLANESPE vec4(1.0, 1.0, 1.0, 1.0)
#define PLANESHI 10

GLuint sphereVao;

// object properties
#pragma endregion

#pragma region lighting

point4 lightPos =  point4( 0.0, 0.0, 2.0, 1.0 );

#define LIGHTDEF point4( 0.0, 0.0, 2.0, 1.0 )
#define LIGHTAMB color4( 0.2, 0.2, 0.2, 1.0 )
#define LIGHTDIF color4( 1.0, 1.0, 1.0, 1.0 )
#define LIGHTSPE color4( 1.0, 1.0, 1.0, 1.0 )

//lighting
#pragma endregion 

#pragma region sphere

int NumVertices = 0;

std::vector<vec4> points;
std::vector<vec3> normals;
std::vector<vec2> tex_coord;

//sphere
#pragma endregion

//GLFW functions
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}


//TODO get these arguements sorted out
void genPoint(int i, int j, int m, int n){
	vec3 p = vec3(sin(M_PI * (float(j) / m)) * cos(2 * M_PI * (float(i) / n)),
				sin(M_PI * (float(j) / m)) * sin(2 * M_PI * (float(i) / n)),
				cos(M_PI * (float(j) / m)));
	vec3 nor = normalize(p);
	points.push_back(vec4(p, 1.0));
	normals.push_back(nor);
	tex_coord.push_back(vec2(.5 + atan2(-nor.z, -nor.x) / (M_PI * 2),
		.5 - asin(-nor.y) / M_PI));

}

//Create a sphere from long. (m) and lang. (n) parameters
void genSphere(int m, int n, int r)
{

	for (int i = 0; i < n; ++i){
		for (int j = 1; j <= m; ++j){

			genPoint(i + 1, j, m, n);

			genPoint(i, j, m, n);

			genPoint(i, j - 1, m, n);

			genPoint(i + 1, j - 1, m, n);

			genPoint(i + 1, j, m, n);

			genPoint(i, j - 1, m, n);
		}
	}
	//create texture data
	//this is the default color texture
	glActiveTexture(GL_TEXTURE0);

	GLuint tex_ld = SOIL_load_OGL_texture
		("BeachBallColor.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_NTSC_SAFE_RGB
		);

	if (0 == tex_ld)
	{
		printf("SOIL loading error: '%s'\n", SOIL_last_result());
		exit(EXIT_FAILURE);
	}
	
	glBindTexture(GL_TEXTURE_2D, tex_ld);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_RGB, GL_FLOAT, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glGenVertexArrays(1, &sphereVao);
	glBindVertexArray(sphereVao);


	//get arrays from vector data structures

	NumVertices = points.size();

	int sizeof_points = points.size() * sizeof(vec4);
	int sizeof_normals = normals.size() * sizeof(vec3);
	int sizeof_tex = tex_coord.size() * sizeof(vec2);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof_points+sizeof_normals+sizeof_tex, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof_points, &points[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof_points, sizeof_normals, &normals[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof_points+sizeof_normals, sizeof_tex, &tex_coord[0]);

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof_points));

	GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof_points+sizeof_normals));

	glUniform1i(glGetUniformLocation(program, "textureColor"), 0);

}


// OpenGL initialization
void init()
{
	// Load shaders and use the resulting shader program
	program = InitShader("vshaderTexture.glsl", "fshaderTexture.glsl");

	glUseProgram(program);

	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");
	AmbientProduct = glGetUniformLocation(program, "AmbientProduct");
	DiffuseProduct = glGetUniformLocation(program, "DiffuseProduct");
	SpecularProduct = glGetUniformLocation(program, "SpecularProduct");
	LightPosition = glGetUniformLocation(program, "LightPosition");
	Shininess = glGetUniformLocation(program, "Shininess");
	Rot = glGetUniformLocation(program, "Rot");


	//Setup the view volume with Perspective
	if (projType == ORTHO)
		proj = Ortho(left, right, bottom, top, zoNear, zoFar);
	else
		proj = Perspective(fovy, aspect, zpNear, zpFar);

	//calculate matrices
	mv = LookAt(eye, at, up);

	rot = vec4(1, 0, 0, 0);


	glUniform4fv(LightPosition, 1, lightPos);
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, mv);
	glUniformMatrix4fv(Projection, 1, GL_TRUE, proj);
	glUniform4fv(AmbientProduct, 1, LIGHTAMB);
	glUniform4fv(DiffuseProduct, 1, LIGHTDIF);
	glUniform4fv(SpecularProduct, 1, LIGHTSPE);
	glUniform1f(Shininess, 30000);

	genSphere(40, 80, 1);

	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);

	glClearColor(1.0, 1.0, 1.0, 1.0);
}

//Display function
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glBindVertexArray(sphereVao);
	glUniform4fv(Rot, 1, rot);

	glDrawArrays(GL_TRIANGLES, 0 , NumVertices);

	glBindVertexArray(0);
	
}

//GLFW mouse function
void mouseButton( GLFWwindow *window, int button, int action, int mods)
{
	glfwGetCursorPos(window, &xCur, &yCur);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
		buttonHeld = true;
		velocity = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
		buttonHeld = false;
		velocity = true;
	}
}

vec4 q_multiply(vec4 a, vec4 b){

	float vDot = dot(vec3(a.y, a.z, a.w), vec3(b.y, b.z, b.w));
	vec3 vCross = cross(vec3(a.y, a.z, a.w), vec3(b.y, b.z, b.w));
	vec3 v1 = a.x * vec3(b.y, b.z, b.w);
	vec3 v2 = b.x * vec3(a.y, a.z, a.w);
	return vec4(a.x * b.x - vDot, vCross.x + v1.x + v2.x,
							      vCross.y + v1.y + v2.y, 
								  vCross.z + v1.z + v2.z );
}


vec3 hemisphereMap(double x, double y){
	double xAdj = (2 * x - screenWidth) / screenWidth;
	double yAdj = (screenHeight - 2 * y) / screenHeight;
	float len = sqrt(xAdj*xAdj + yAdj*yAdj);
	len = (len < 1.0) ? len : 1.0;
	return normalize(vec3(xAdj, yAdj, sqrt(1.001 - len * len)));


}

void rotate(double x0, double y0, double x1, double y1, double speed){
	vec3 init = hemisphereMap(x0, y0);
	vec3 fin = hemisphereMap(x1, y1);

	vec3 n = cross(init, fin);

	vec3 axis = normalize(n);
	float mag = speed * length(n);


	float s = sin(mag / 2);
	vec4 nRot = vec4(cos(mag / 2), axis.x * s, axis.y * s, axis.z * s);
	rot = q_multiply(nRot, rot);
}

//GLFW mouseMotion function
void mouseMotion(GLFWwindow* window, double xPos, double yPos)
{
	if(buttonHeld) {
		xPrev = xCur;
		yPrev = yCur;
		rotate(xPrev, yPrev, xPos, yPos, 1.0);
		xCur = xPos;
		yCur = yPos;
	}
}

//GLFW resize func
void windowFunc(GLFWwindow*, int w, int h){
	glViewport(0, 0, w, h);

	float ar = (float)(w) / h;
	screenHeight = h;
	screenWidth = w;


	proj = Perspective(fovy, ar, zpNear, zpFar);
	glUniformMatrix4fv(Projection, 1, GL_TRUE, proj);



}

//GLFW idle function
void idle()
{
	if (velocity){
		rotate(xPrev, yPrev, xCur, yCur, 0.01);
	}
}

//main function
int main( int argc, char **argv )
{

	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	window = glfwCreateWindow(screenHeight, screenWidth, "Caleb Bauermeister : Homework 1", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouseButton);
	glfwSetCursorPosCallback(window, mouseMotion);
	glfwSetWindowSizeCallback(window, windowFunc);

	GLint GlewInitResult = glewInit();
	if (GLEW_OK != GlewInitResult)
	{
		printf("ERROR: %s\n", glewGetErrorString(GlewInitResult));
		exit(EXIT_FAILURE);
	}
	
	init();

	while (!glfwWindowShouldClose(window)){

		idle();
		display();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
