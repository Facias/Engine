

#include <stdio.h>
#include <string>

#include <iostream>
#include <fstream>
#include <algorithm>


#include <stdlib.h>
#include <string.h>
#include "stack.h"

#include "Shader.h"


using namespace ENGINE;

Shader::Shader(const Shader &other)
{

	vertexName = other.vertexName;
	fragName = other.fragName;


	id = other.id;

	uniformMax = other.uniformMax;
	uniformCount = other.uniformCount;

	for (int n = 0; n < 16; n++)
	{
		textureLocation[n] = other.textureLocation[n];
	}

	for (int n = 0; n < uniformMax; n++)
	{
		strcpy(uniformName[n], other.uniformName[n]);
		uniformLocation[n] = other.uniformLocation[n];
		uniformType[n] = other.uniformType[n];
	}
}

GLuint ENGINE::LoadShaders(const char * vertex_file_path, const char * fragment_file_path)
{

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open())
	{
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else 
	{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) 
	{
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		Stack<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage.item[0]);
		printf("%s\n", &VertexShaderErrorMessage.item[0]);
	}


	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) 
	{
		Stack<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage.item[0]);
		printf("%s\n", &FragmentShaderErrorMessage.item[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) 
	{
		Stack<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage.item[0]);
		printf("%s\n", &ProgramErrorMessage.item[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

void Shader::compileUniforms()
{
	glUseProgram(id);

	FILE *file;
	file = fopen(vertexName, "r");
	
	if (file)
	{

		while (1)
		{

			char prefix[16384];
			char typeSTR[16384];
			char name[16384];
			
			int read = fscanf(file, "%s %s %[^;];\n", prefix, typeSTR, name);
			if (read == EOF) break;
			if (strcmp(prefix, "uniform") == 0 )
			{
				
				
				printf("%s %s %s\n", prefix, typeSTR, name);

				if (strcmp(typeSTR, "bool") == 0)
				{
					addUniform(name, S_BOOL);
				}
				if (strcmp(typeSTR, "int") == 0)
				{
					addUniform(name, S_INT);
				}
				if (strcmp(typeSTR, "float") == 0)
				{
					addUniform(name, S_FLOAT);
				}
				if (strcmp(typeSTR, "vec2") == 0)
				{
					addUniform(name, S_VEC2);
				}
				if (strcmp(typeSTR, "vec3") == 0)
				{
					addUniform(name, S_VEC3);
				}
				if (strcmp(typeSTR, "vec4") == 0)
				{
					addUniform(name, S_VEC4);
				}
				if (strcmp(typeSTR, "mat4") == 0)
				{
					addUniform(name, S_MAT4);
					continue;
				}
				printf("Unidentified type %s \n", typeSTR);
				// samplers not added in this part
			}

		}
	}
	fclose(file);

	file = fopen(fragName, "r");
	if (file)
	{
		while (1)
		{
			char prefix[16384];
			char typeSTR[16384];
			char name[16384];

			int read = fscanf(file, "%s %s %[^;];\n", prefix, typeSTR, name);
			if (read == EOF) break;
			if (strcmp(prefix, "uniform") == 0)
			{
				printf("%s %s %s\n" ,prefix, typeSTR, name);

				if (strcmp(typeSTR, "bool") == 0)
				{
					addUniform(name, S_BOOL);
				}
				if (strcmp(typeSTR, "int") == 0)
				{
					addUniform(name, S_INT);
				}
				if (strcmp(typeSTR, "float") == 0)
				{
					addUniform(name, S_FLOAT);
				}
				if (strcmp(typeSTR, "vec2") == 0)
				{
					addUniform(name, S_VEC2);
				}
				if (strcmp(typeSTR, "vec3") == 0)
				{
					addUniform(name, S_VEC3);
				}
				if (strcmp(typeSTR, "vec4") == 0)
				{
					addUniform(name, S_VEC4);
				}
				

				// samplers not added in this part
			}

		}
	}
	fclose(file);
}

void Shader::getTextureLocations(float samples)
{
	glUseProgram(id);

	textureLocation[0] = glGetUniformLocation(id, "texture0");
	glUniform1i(textureLocation[0], 0);

	textureLocation[1] = glGetUniformLocation(id, "texture1");
	glUniform1i(textureLocation[1], 1);

	textureLocation[2] = glGetUniformLocation(id, "texture2");
	glUniform1i(textureLocation[2], 2);

	textureLocation[3] = glGetUniformLocation(id, "texture3");
	glUniform1i(textureLocation[3], 3);

	textureLocation[4] = glGetUniformLocation(id, "texture4");
	glUniform1i(textureLocation[4], 4);

	textureLocation[5] = glGetUniformLocation(id, "texture5");
	glUniform1i(textureLocation[5], 5);

	textureLocation[6] = glGetUniformLocation(id, "texture6");
	glUniform1i(textureLocation[6], 6);

	textureLocation[7] = glGetUniformLocation(id, "texture7");
	glUniform1i(textureLocation[7], 7);

	textureLocation[8] = glGetUniformLocation(id, "texture8");
	glUniform1i(textureLocation[8], 8);

	textureLocation[9] = glGetUniformLocation(id, "texture9");
	glUniform1i(textureLocation[9], 9);

	textureLocation[10] = glGetUniformLocation(id, "texture10");
	glUniform1i(textureLocation[10], 10);

	textureLocation[11] = glGetUniformLocation(id, "texture11");
	glUniform1i(textureLocation[11], 11);

	textureLocation[12] = glGetUniformLocation(id, "texture12");
	glUniform1i(textureLocation[12], 12);

	textureLocation[13] = glGetUniformLocation(id, "texture13");
	glUniform1i(textureLocation[13], 13);

	textureLocation[14] = glGetUniformLocation(id, "texture14");
	glUniform1i(textureLocation[14], 14);

	textureLocation[15] = glGetUniformLocation(id, "texture15");
	glUniform1i(textureLocation[15], 15);

	textureLocation[16] = glGetUniformLocation(id, "texture16");
	glUniform1i(textureLocation[16], 16);

	//textureLocation[16] = glGetUniformLocation(id, "texture16");
	//glUniform1i(textureLocation[16], 16);
}

GLint Shader::addUniform( char *name, int s_type)
{

	for (int n = 0; n < uniformCount+1; n++)
	{
		if (uniformName[n] != NULL)
		{
			int a = strcmp(name, uniformName[n]);
			if (a == 0) // uniform exists
			{
				return uniformLocation[n];
			}
		}
	}
	// if uniform does not already exist, get new location


	uniformLocation[uniformCount] = glGetUniformLocation(id, name);
	strcpy(uniformName[uniformCount], name);
	uniformType[uniformCount] = s_type;

	uniformCount++;

	return uniformLocation[uniformCount-1];
}

GLint Shader::getUniformLocation(char *name)
{
	for (int n = 0; n < uniformCount; n++)
	{
		//if (uniformName[n] != NULL)
		{
			int a = strcmp(name, uniformName[n]);
			if (a == 0) // uniform exists
			{
				return uniformLocation[n];
			}
		}
	}
	return -2;
}

unsigned char Shader::getUniformType(char *name)
{
	
	for (int n = 0; n < uniformCount; n++)
	{
		if (uniformName[n] != NULL)
		{
			int a = strcmp(name, uniformName[n]);
			if (a == 0) // uniform exists
			{
				return uniformType[n];
			}
		}
	}

	return S_UNDEFINED;
}

int ShaderManager::loadShader(char *vertex, char* fragment)
{
	
	//check to see if shader already exists
	for (int n = 0; n < shaderCount; n++)
	{
		if (shaderArray[n].vertexName != NULL)
		{

			int a = strcmp(shaderArray[n].vertexName, vertex);
			if (a == 0) // vertex is the same. if the fragment matches, we return this existing shader 
			{
				a = strcmp(shaderArray[n].fragName, fragment);
				if (a == 0) return n;
			}
		}
	}
	// if we're still here that means the shader doesn't exist. proceed in loading

	Shader temp;
	temp.id = LoadShaders(vertex, fragment);
	temp.getTextureLocations(samples);
	temp.fragName = fragment;
	temp.vertexName = vertex;

	temp.compileUniforms();


	return add(temp);

}

int ShaderManager::add(Shader shader)
{
	if (shaderCount + 2 >= arraySize)
	{
		Shader *temp = new Shader[arraySize * 2];
		for (int n = 0; n < shaderCount; n++)
			temp[n] = shaderArray[n];

		delete[] shaderArray;
		arraySize *= 2;
		shaderArray = temp;

		printf("COMPLETED DOUBLE %d \n", arraySize);
	}
	for (int n = 0; n < shaderCount; n++)
	{
		if (shaderArray[n].fragName == NULL)
		{
			shaderArray[n] = shader;
			printf(shader.fragName);
			printf("COMPLETED ADD %d \n", n);
			return n;
		}
	}
	shaderArray[shaderCount] = shader;
	shaderCount++;
	printf("COMPLETED ADD %d \n", shaderCount);
	return shaderCount-1;
}

void ShaderManager::setMultisampleState(float ms)
{
	for (int n = 0; n < shaderCount; n++)
	{
		//shaderArray[n].getTextureLocations(ms);
	}
	//samples = ms;
}

void ShaderManager::remove(int index)
{
	shaderArray[index] = Shader();
}