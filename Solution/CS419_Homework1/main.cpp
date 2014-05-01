//
//  Display a scene comprised of various .obj files
//
//  Light and material properties are sent to the shader as uniform
//    variables.  Vertex positions and normals are sent after each
//    rotation.

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
GLuint Trans;
GLuint AmbientProduct;
GLuint DiffuseProduct;
GLuint SpecularProduct;
GLuint LightPosition;
GLuint Shininess;

//vector to hold programs
std::vector<GLuint> programs;

//Gluints
#pragma endregion

#pragma region matrices

//Model-view and projection matrices
mat4  mv, proj, ms, ct;

//matrices
#pragma endregion 

#pragma region camera

point4  eye = point4(0.0, 0.0, 2.0, 1.0);
point4  at  = point4(0.0, 0.0, 0.0, 1.0);
vec4    up  = vec4  (0.0, 1.0, 0.0, 0.0);

point4 eye0 = eye;
point4 at0  = at;
vec4 up0    = up;

projection projType = PERSPEC;

//camera
#pragma endregion

#pragma region ui elements

bool buttonHeld = false;

int mode = 0; //default mode

bool bump = true;
bool rotateCube = true;
bool rotateWithCube = false;
int cubeMode = 0;

//menu ids
int top_menu;
int scene_menu;
int	rot_menu;
int bump_cube_menu;

int screenWidth  = 512;
int screenHeight = 512;

int xPrev, yPrev;

//ui elements
#pragma endregion 

#pragma region object properties

#define PLANEAMB vec4(.658824, .658824 , .658824,  1.0)
#define PLANEDIF vec4(.658824, .658824 , .658824,  1.0)
#define PLANESPE vec4(1.0, 1.0, 1.0, 1.0)
#define PLANESHI 10

GLuint cubeVao;

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

#pragma region texture cube

GLuint textureBump;
GLuint textureCube;

int NumVertices = 0;
const int TextureSize = 128;
vec3 image[TextureSize][TextureSize];

std::vector<vec4> points;
std::vector<vec3> normals;
std::vector<vec3> tangents;
std::vector<vec2> tex_coord;

vec3 bumpNormals[TextureSize][TextureSize];

float data[TextureSize][TextureSize];

//texture cube
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

//for random numbers
float Ranf( float low, float high )
{
	long random();		// returns integer 0 - TOP
	float r;		// random number	

	r = (float)rand();

	return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}

//used to 
void switchShaders(int n)
{
	glUseProgram(programs[n]);

	ModelView       = glGetUniformLocation( programs[n], "ModelView"      );
	Projection      = glGetUniformLocation( programs[n], "Projection"     );
	Trans           = glGetUniformLocation( programs[n], "Trans"          );
	AmbientProduct  = glGetUniformLocation( programs[n], "AmbientProduct" );
	DiffuseProduct  = glGetUniformLocation( programs[n], "DiffuseProduct" );
	SpecularProduct = glGetUniformLocation( programs[n], "SpecularProduct");
	LightPosition   = glGetUniformLocation( programs[n], "LightPosition"  );
	Shininess       = glGetUniformLocation( programs[n], "Shininess"      );

	glUniform4fv( LightPosition, 1, lightPos);
	glUniformMatrix4fv( ModelView, 1, GL_TRUE, mv );
	glUniformMatrix4fv( Projection, 1, GL_TRUE, proj );

}

//TODO get these arguements sorted out
vec3 genPoint(int i, int j, int m, int n){
	return vec3(sin(M_PI * (float(j) / m)) * cos(2 * M_PI * (float(i) / n)),
				sin(M_PI * (float(j) / m)) * sin(2 * M_PI * (float(i) / n)),
				cos(M_PI * (float(j) / m)));
}

//create a single layer of the sphere
void genLayer(int i, int n, int m, int r)
{

	for (int j = 1; j <= m; ++j){


		vec3 p = genPoint(i + 1, j, m, n);
		vec3 nor = normalize(p);
		points.push_back(vec4(p, 1.0));
		normals.push_back(nor);
		tex_coord.push_back(vec2(.5 + atan2(-nor.z, -nor.x) / M_PI * 2,
			.5 - asin(-nor.y) / M_PI));

		p = genPoint(i, j, m, n);
		nor = normalize(p);
		points.push_back(vec4(p, 1.0));
		normals.push_back(nor);
		tex_coord.push_back(vec2(.5 + atan2(-nor.z, -nor.x) / M_PI * 2,
			.5 - asin(-nor.y) / M_PI));

		p = genPoint(i, j - 1, m, n);
		nor = normalize(p);
		points.push_back(vec4(p, 1.0));
		normals.push_back(nor);
		tex_coord.push_back(vec2(.5 + atan2(-nor.z, -nor.x) / M_PI * 2,
			.5 - asin(-nor.y) / M_PI));

		p = genPoint(i + 1, j - 1, m, n);
		nor = normalize(p);
		points.push_back(vec4(p, 1.0));
		normals.push_back(nor);
		tex_coord.push_back(vec2(.5 + atan2(-nor.z, -nor.x) / M_PI * 2,
			.5 - asin(-nor.y) / M_PI));


		p = genPoint(i + 1, j, m, n);
		nor = normalize(p);
		points.push_back(vec4(p, 1.0));
		normals.push_back(nor);
		tex_coord.push_back(vec2(.5 + atan2(-nor.z, -nor.x) / M_PI * 2,
			.5 - asin(-nor.y) / M_PI));



		p = genPoint(i, j - 1, m, n);
		nor = normalize(p);
		points.push_back(vec4(p, 1.0));
		normals.push_back(nor);
		tex_coord.push_back(vec2(.5 + atan2(-nor.z, -nor.x) / M_PI * 2,
			.5 - asin(-nor.y) / M_PI));
	}


}

void genSphere(int m, int n, int r)
{

	for (int i = 0; i < n; ++i){
		genLayer(i, n, m, r);
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
	switchShaders(0);

	glGenVertexArrays(1, &cubeVao);
	glBindVertexArray(cubeVao);


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

	GLuint vPosition = glGetAttribLocation(programs[0], "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint vNormal = glGetAttribLocation(programs[0], "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof_points));

	GLuint vTexCoord = glGetAttribLocation(programs[0], "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof_points+sizeof_normals));

	glUniform1i(glGetUniformLocation(programs[0], "textureColor"), 0);

}


// OpenGL initialization
void init()
{
	// Load shaders and use the resulting shader program
	GLuint programTemp = InitShader("vshaderTexture.glsl", "fshaderTexture.glsl");
	programs.push_back(programTemp);

	//programTemp = InitShader( "vshaderFinalTexture.glsl", "fshaderFinalTexture.glsl");
	//programs.push_back(programTemp);

	//calculate matrices
	ms = identity();
	ct = identity();
	mv = LookAt(eye, at, up);

	//Setup the view volume with Perspective
	if (projType == ORTHO)
		proj = Ortho(left, right, bottom, top, zoNear, zoFar);
	else
		proj = Perspective(fovy, aspect, zpNear, zpFar);

	genSphere(80, 80, 1);

	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);

	glClearColor(1.0, 1.0, 1.0, 1.0);
}

//GLUT display function
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	switchShaders(0);

	color4 ambient_product  = LIGHTAMB * PLANEAMB;
	color4 diffuse_product  = LIGHTDIF * PLANEDIF;
	color4 specular_product = LIGHTSPE * PLANESPE;

	glBindVertexArray(cubeVao);
	glUniform4fv(AmbientProduct, 1, ambient_product);
	glUniform4fv(DiffuseProduct, 1, diffuse_product);
	glUniform4fv(SpecularProduct, 1, specular_product);
	glUniform1f( Shininess, 300 );
	glUniformMatrix4fv(Trans, 1, GL_TRUE, identity());

	glDrawArrays(GL_TRIANGLES, 0 , NumVertices);

	glBindVertexArray(0);
	
}

	
//GLUT mouse function
void mouse( GLFWwindow *window, int button, int action, int mods)
{
	double xDub = 0;
	double yDub = 0;

	glfwGetCursorPos(window, &xDub, &yDub);

	int x = (int)xDub;
	int y = (int)yDub;

	y = screenHeight - y;

	
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		buttonHeld = true;
	else
		buttonHeld = false;


	xPrev = (int)x;
	yPrev = (int)y;

	

	
}

//GLUT mouseMotion function
void mouseMotion(GLFWwindow* window, double xDub, double yDub)
{

	int x = (int)xDub;
	int y = (int)yDub;

	y = screenHeight - y;
	mat4 temp = identity();

	//Camera Dolly
	if(buttonHeld && mode == 4)
		temp = Translate(0, 0, (yPrev - y) / (float)screenHeight);
	//Camera translate
	else if(buttonHeld && mode == 3)
		temp = Translate((xPrev - x) / (float)screenHeight, (yPrev - y) / (float)screenHeight, 0);
	//Camera Rotate Z
	else if(buttonHeld && mode == 2)
		temp = RotateZ((x - xPrev));
	//Camera Rotate Y
	else if(buttonHeld && mode == 1)
		temp = RotateY((x - xPrev));
	//Camera Rotate X
	else if(buttonHeld && mode == 0) 
		temp = RotateX((yPrev - y));


	if(buttonHeld && (mode == 0 || mode == 1 || mode == 2 || mode == 3 || mode == 4)) {

		ms = ms * temp;

		at   = ms * at0;
		eye  = ms * eye0;
		up   = ms * up0;

		mv = LookAt( eye, at, up );

		//update our shader uniforms
		for(unsigned i = 0; i < programs.size(); ++i)
			switchShaders(i);
	}

	xPrev = x;
	yPrev = y;

}

//idle function
void idle()
{

}

//main function
int main( int argc, char **argv )
{

	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	window = glfwCreateWindow(640, 640, "Caleb B : Homework 1", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse);
	glfwSetCursorPosCallback(window, mouseMotion);

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
