#ifndef __OPENGL_UTIL__
#define __OPENGL_UTIL__

#define GLFW_INCLUDE_GLU
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdlib>


#ifndef M_PI
#  define M_PI  3.14159265358979323846
#endif

// Define a helpful macro for handling offsets into buffer objects
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

//  Defined constant for when numbers are too small to be used in the
//    denominator of a division operation.  This is only used if the
//    DEBUG macro is defined.
const GLfloat  DivideByZeroTolerance = GLfloat(1.0e-07);

//  Degrees-to-radians constant
const GLfloat  DegreesToRadians = M_PI / 180.0;


enum projection{ ORTHO, PERSPEC };


#include "vec.h"
#include "mat.h"

//provided  methods for  reading shaders, modified by myself to use C++ iostreams rather than C i/o

static char* readShaderSource(const char* shaderFile);

GLuint InitShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile = NULL);

#endif //__OPENGL_UTIL__