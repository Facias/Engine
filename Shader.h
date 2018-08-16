#ifndef SHADER_H_
#define SHADER_H_

#define _CRT_SECURE_NO_WARNINGS

#include <glew.h>
#include "glm.hpp"


namespace ENGINE
{

	struct Shader
	{
		Shader()
		{
			fragName = NULL;
			vertexName = NULL;
			id = 0;

			uniformMax = 32;
			uniformCount = 0;

			for (int n = 0; n < 32; n++)
			{
				textureLocation[n] = -2;
			}

			for (int n = 0; n < uniformMax; n++)
			{
				//uniformName[n] = NULL;
				uniformLocation[n] = -2;
				uniformLocation[n] = S_UNDEFINED;
			}

		}
		Shader(const Shader &other);
		

		enum S_TYPE
		{
			S_BOOL,
			S_INT,
			S_FLOAT,
			S_VEC2,
			S_VEC3,
			S_VEC4,
			S_MAT2,
			S_MAT3,
			S_MAT4,
			S_UNDEFINED
		};

		char *vertexName;
		char *fragName;

		// save all unique uniform names and locations.
		// we don't what to have to find locations unnecessarily,
		// since it is very slow
		char uniformName[32][256];
		GLint uniformLocation[32];
		
		// size of 1 for scalar, 2 for vec2, 3 for vec3 ....
		unsigned char uniformType[32];

		unsigned char uniformMax;
		unsigned char uniformCount;


		
		// look to see if uniform location already set.
		// if not, add it.
		// return location of this uniform
		GLint addUniform( char *name, int s_type);
		GLint getUniformLocation(char *name);
		unsigned char getUniformType(char *name);
		//void getUniformInfo(char *name, GLint *location, unsigned char *type);

		GLint textureLocation[32];

		void getTextureLocations(float samples);

		// look through shader files and determine
		// what uniforms will be used, and what size they are
		void compileUniforms();



		GLuint id;
	};

	
	struct ShaderConst
	{
		ShaderConst()
		{
			name = NULL;
			v[0] = 0.0f;
			v[1] = 0.0f;
			v[2] = 0.0f;
			v[3] = 0.0f;
			location = -1;
			s_type = Shader::S_VEC4;
		}
		ShaderConst(char *n_name, float nv0, float nv1, float nv2, float nv3, Shader::S_TYPE type)
		{
			name = n_name;
			v[0] = nv0;
			v[1] = nv1;
			v[2] = nv2;
			v[3] = nv3;
			location = -1;
			s_type = type;
		}
		ShaderConst(const ShaderConst &other)
		{
			name = other.name;
			v[0] = other.v[0];
			v[1] = other.v[1];
			v[2] = other.v[2];
			v[3] = other.v[3];
			location = other.location;
			s_type = other.s_type;
		}

		char *name;

		float v[4];
		GLint location;
		unsigned char s_type;

	};

	struct ShaderMat
	{
		ShaderMat()
		{
			name = NULL;
			value = glm::mat4(1);
			location = -1;
		}
		ShaderMat(char *n_name, glm::mat4 v, Shader::S_TYPE type)
		{
			name = n_name;
			value = v;
			location = -1;
		}
		ShaderMat(const ShaderMat &other)
		{
			name = other.name;
			value = other.value;
			location = other.location;
		}

		char *name;

		glm::mat4 value;
		GLint location;
	};

	GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);

	struct ShaderManager
	{
		ShaderManager()
		{
			shaderArray = new Shader[32];
			arraySize = 32;
			samples = 0.0;
			shaderCount = 0;
		}
		ShaderManager(const ShaderManager &other)
		{
			shaderArray = other.shaderArray;
			shaderCount = other.shaderCount;
			arraySize = other.arraySize;
			samples = other.samples;
		}

		Shader * shaderArray;
		unsigned short int arraySize; // max capacity
		unsigned short int shaderCount; // current number

		//return the location of addition
		int add(Shader shader); 
		void remove(int index);
		
		float samples;
		void setMultisampleState(float samples);

		// return the index of the shader in array
		int loadShader(char *vertex, char* fragment); 
	};
}
#endif