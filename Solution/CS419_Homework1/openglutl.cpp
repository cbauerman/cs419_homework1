#include <iostream>
#include <fstream>
#include "openglutl.h"

static char* readShaderSource(const char* shaderFile)
{
	std::ifstream file(shaderFile, std::ifstream::binary);

	if( file == NULL )
		return NULL;

	//fseek(fp, 0L, SEEK_END);
	file.seekg(0, std::ios::end);
	long size = file.tellg();

	file.seekg(0, std::ios::beg);
	char* buf = new char[size + 1];
	file.read(buf, size);

	buf[size] = '\0';

	file.close();
	return buf;
}

GLuint InitShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile)
{
	struct Shader{
		const char* filename;
		GLenum		type;
		GLchar*     source;
	} shaders[3] = {
		{ vShaderFile, GL_VERTEX_SHADER, NULL },
		{ fShaderFile, GL_FRAGMENT_SHADER, NULL},
		{ gShaderFile, GL_GEOMETRY_SHADER, NULL}
	};


	GLuint program = glCreateProgram();

	int j = 0;

	if(gShaderFile == NULL)
	{
		j = 2;
	}
	else
	{
		j = 3;
	}

	for(int i = 0; i < j; ++i)
	{
		Shader& s = shaders[i];

		s.source = readShaderSource( s.filename );

		if( shaders[i].source == NULL )
		{
			std::cout << "Failed to read " << s.filename << std::endl;
			exit(EXIT_FAILURE);
		}

		GLuint shader = glCreateShader( s.type );
		glShaderSource( shader, 1, (const GLchar**) &s.source, NULL);
		glCompileShader( shader );

		GLint compiled;

		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled );

		if ( !compiled)
		{
			std::cout << s.filename << "failed to compile:" << std::endl;
			GLint logSize;

			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize );
			char* logMsg = new char [logSize];
			glGetShaderInfoLog( shader, logSize, NULL, logMsg );
			std::cout << logMsg << std::endl;

			delete [] logMsg;
			 exit (EXIT_FAILURE);
		}

		delete [] s.source;

		glAttachShader(program, shader);
	}

	glLinkProgram(program);

	GLint linked;
	glGetProgramiv( program, GL_LINK_STATUS, &linked);

	if (!linked )
	{
		std::cout << "Shader program failed to link" << std::endl;
		GLint logSize;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
		char* logMsg = new char[logSize];
		glGetProgramInfoLog(program, logSize, NULL, logMsg);
		std::cout << logMsg << std::endl;
		delete [] logMsg;

		exit( EXIT_FAILURE );
	}

	glUseProgram(program);

	return program;
}