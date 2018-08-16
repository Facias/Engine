#include "Particle.h"
#include <string>

using namespace ENGINE;

ParticleSet::ParticleSet()
{
	pos = NULL;
	rot = NULL;
	scale = NULL;

	count = 0;

	posBufferID = 0;
	shaderIndex = 0;

	constantCount = 0;
	constantMatCount = 0;

	culling = 1;
	visible = true;
	transparent = false;
	castShadow = false;
	follow = false;
	reflectable = true;

	genPlane = true;
}

ParticleSet::ParticleSet(int size)
{
	pos = new glm::vec3[size];
	rot = new glm::vec3[size];
	scale = new glm::vec3[size];
	color = new glm::vec3[size];

	for (int n = 0; n < size; n++)
	{
		scale[n] = glm::vec3(1);
		color[n] = glm::vec3(1);

	}

	count = size;

	glGenBuffers(1, &posBufferID);
	glGenBuffers(1, &rotBufferID);
	glGenBuffers(1, &scaleBufferID);
	glGenBuffers(1, &colorBufferID);

	for (int n = 0; n < 16; n++)
	{
		constantArray[n] = ShaderConst();
		constantMatArray[n] = ShaderMat();
	}
	for (int n = 0; n < 8; n++)
	{
		texture[n] = 0;
	}

	constantCount = 0;
	constantMatCount = 0;

	culling = 1;
	visible = true;
	transparent = false;
	castShadow = false;
	follow = false;
	reflectable = true;

	blendType = 0;

	genPlane = true;
}

ParticleSet::ParticleSet(const ParticleSet &other)
{
	pos = other.pos;
	rot = other.rot;
	scale = other.scale;
	color = other.color;

	posBufferID = other.posBufferID;
	rotBufferID = other.rotBufferID;
	scaleBufferID = other.scaleBufferID;
	colorBufferID = other.colorBufferID;

	shaderIndex = other.shaderIndex;

	for (int n = 0; n < 32; n++)
	{
		constantArray[n] = other.constantArray[n];

		if (n<16)
			constantMatArray[n] = other.constantMatArray[n];
	}
	for (int n = 0; n < 8; n++)
	{
		texture[n] = other.texture[n];
	}

	constantCount = other.constantCount;
	constantMatCount = other.constantMatCount;

	count = other.count;

	culling = other.culling;
	visible = other.visible;
	transparent = other.transparent;
	castShadow = other.castShadow;
	follow = other.follow;
	reflectable = other.reflectable;

	genPlane = other.genPlane;
	blendType = other.blendType;
}

void ParticleSet::setShaderConst(Shader *shader, char *name, float v0, float v1, float v2, float v3)
{
	GLint loc;
	unsigned char type;

	loc = shader->getUniformLocation(name);
	type = shader->getUniformType(name);

	// check if uniform name already exists on this object
	for (int n = 0; n < constantCount; n++)
	{
		if (constantArray[n].name != NULL)
		{
			if (strcmp(name, constantArray[n].name) == 0)
			{
				constantArray[n].v[0] = v0;
				constantArray[n].v[1] = v1;
				constantArray[n].v[2] = v2;
				constantArray[n].v[3] = v3;
				constantArray[n].location = loc;
				constantArray[n].s_type = type;

				return;
			}
		}
	}

	// if uniform name does not already exist... add it
	constantArray[constantCount].name = name;
	constantArray[constantCount].v[0] = v0;
	constantArray[constantCount].v[1] = v1;
	constantArray[constantCount].v[2] = v2;
	constantArray[constantCount].v[3] = v3;
	constantArray[constantCount].location = loc;
	constantArray[constantCount].s_type = type;

	constantCount++;

	return;
}

void ParticleSet::setShaderMat(Shader *shader, char *name, glm::mat4 val)
{
	GLint loc;
	loc = shader->getUniformLocation(name);

	// check if uniform name already exists on this object
	for (int n = 0; n < constantMatCount; n++)
	{
		if (constantMatArray[n].name != NULL)
		{
			if (strcmp(name, constantMatArray[n].name) == 0)
			{
				constantMatArray[n].value = val;
				constantMatArray[n].location = loc;

				return;
			}
		}
	}

	// if uniform name does not already exist... add it
	constantMatArray[constantMatCount].name = name;
	constantMatArray[constantMatCount].value = val;
	constantMatArray[constantMatCount].location = loc;

	constantMatCount++;

	return;
}

void ParticleSet::updateShaderUniforms(Shader *shader)
{
	for (int n = 0; n < constantCount; n++)
	{

		if (constantArray[n].s_type == Shader::S_FLOAT)
		{
			glUniform1f(constantArray[n].location,
				constantArray[n].v[0]);
		}
		if (constantArray[n].s_type == Shader::S_VEC2)
		{
			glUniform2f(constantArray[n].location,
				constantArray[n].v[0],
				constantArray[n].v[1]);
		}
		if (constantArray[n].s_type == Shader::S_VEC3)
		{
			glUniform3f(constantArray[n].location,
				constantArray[n].v[0],
				constantArray[n].v[1],
				constantArray[n].v[2]);
		}
		if (constantArray[n].s_type == Shader::S_VEC4)
		{
			glUniform4f(constantArray[n].location,
				constantArray[n].v[0],
				constantArray[n].v[1],
				constantArray[n].v[2],
				constantArray[n].v[3]);
		}
	}
	for (int n = 0; n < constantMatCount; n++)
	{
		glUniformMatrix4fv(constantMatArray[n].location, 1, GL_FALSE, &constantMatArray[n].value[0][0]);
	}
}

int ParticleManager::add(ParticleSet item)
{
	if (setCount + 2 > arraySize)
	{
		ParticleSet *temp;
		temp = new ParticleSet[2 * arraySize];

		for (int n = 0; n < setCount; n++)
			temp[n] = setArray[n];

		delete[] setArray;
		arraySize *= 2;

		setArray = temp;
	}

	setArray[setCount++] = item;

	return setCount - 1;

}

bool ParticleManager::loadOBJ(
	const char * path,
	Stack < glm::vec3 > & out_vertices,
	Stack < glm::vec2 > & out_uvs,
	Stack < glm::vec3 > & out_normals
	)
{
	Stack< unsigned int > vertexIndices, uvIndices, normalIndices;
	Stack< glm::vec3 > temp_vertices;
	Stack< glm::vec2 > temp_uvs;
	Stack< glm::vec3 > temp_normals;

	FILE * file = NULL;
	fopen_s(&file, path, "r");
	if (file == NULL)
	{
		printf("Impossible to open the file !\n");
		return false;
	}

	while (1)
	{

		char lineHeader[1024];
		// read the first word of the line
		int res;
		res = fscanf_s(file, "%s", lineHeader, 1024);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

				   // else : parse lineHeader
		if (strcmp(lineHeader, "v") == 0)
		{
			glm::vec3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.add(vertex*glm::vec3(-1, 1, -1));
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			glm::vec2 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.add(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			glm::vec3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.add(normal*glm::vec3(-1, 1, -1));
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			std::string vertex1, vertex2, vertex3, vertex4;
			unsigned int vertexIndex[20], uvIndex[20], normalIndex[20];
			int a = 0;
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
				&vertexIndex[0], &uvIndex[0], &normalIndex[0],
				&vertexIndex[1], &uvIndex[1], &normalIndex[1],
				&vertexIndex[2], &uvIndex[2], &normalIndex[2],
				&vertexIndex[3], &uvIndex[3], &normalIndex[3],
				&vertexIndex[4], &uvIndex[4], &normalIndex[4],
				&vertexIndex[5], &uvIndex[5], &normalIndex[5],
				&vertexIndex[6], &uvIndex[6], &normalIndex[6],
				&vertexIndex[7], &uvIndex[7], &normalIndex[7],
				&vertexIndex[8], &uvIndex[8], &normalIndex[8]);
			a = 9;
			matches += fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n",
				&vertexIndex[a + 0], &uvIndex[a + 0], &normalIndex[a + 0],
				&vertexIndex[a + 1], &uvIndex[a + 1], &normalIndex[a + 1],
				&vertexIndex[a + 2], &uvIndex[a + 2], &normalIndex[a + 2],
				&vertexIndex[a + 3], &uvIndex[a + 3], &normalIndex[a + 3],
				&vertexIndex[a + 4], &uvIndex[a + 4], &normalIndex[a + 4],
				&vertexIndex[a + 5], &uvIndex[a + 5], &normalIndex[a + 5],
				&vertexIndex[a + 6], &uvIndex[a + 6], &normalIndex[a + 6],
				&vertexIndex[a + 7], &uvIndex[a + 7], &normalIndex[a + 7],
				&vertexIndex[a + 8], &uvIndex[a + 8], &normalIndex[a + 8]);
			if (matches % 3 != 0)
			{
				printf("File can't be read  : ( Try exporting with other options\n");
				return false;
			}


			if (matches >= 9)
			{
				for (int n = 0; n < (matches - 6) / 3; n++)
				{
					vertexIndices.add(vertexIndex[0]);

					vertexIndices.add(vertexIndex[n + 1]);
					vertexIndices.add(vertexIndex[n + 2]);

					uvIndices.add(uvIndex[0]);

					uvIndices.add(uvIndex[n + 1]);
					uvIndices.add(uvIndex[n + 2]);

					normalIndices.add(normalIndex[0]);

					normalIndices.add(normalIndex[n + 1]);
					normalIndices.add(normalIndex[n + 2]);

				}
			}

		}
	}


	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size; i++)
	{
		unsigned int vertexIndex = vertexIndices.item[i];
		glm::vec3 vertex = temp_vertices.item[vertexIndex - 1];
		out_vertices.add(vertex);
	}

	// For each uv of each triangle
	for (unsigned int i = 0; i < uvIndices.size; i++)
	{
		unsigned int uvIndex = uvIndices.item[i];
		glm::vec2 uv = temp_uvs.item[uvIndex - 1];
		out_uvs.add(uv);
	}

	// For each normal of each triangle
	for (unsigned int i = 0; i < normalIndices.size; i++)
	{
		unsigned int normalIndex = normalIndices.item[i];
		glm::vec3 normal = temp_normals.item[normalIndex - 1];
		out_normals.add(normal);
	}

}
/*
int ParticleManager::loadMesh(char *name)
{

	ParticleSet temp;

	char * ext;
	char filename[1024];
	strcpy(filename, name);

	ext = strtok(filename, ".");
	ext = strtok(NULL, ".");


	if (strcmp(ext, "obj") == 0)
	{
		bool exist = false;

		Stack<glm::vec3> vertex;
		Stack<glm::vec2> uv;
		Stack<glm::vec3> normal;
		Stack<glm::vec3> color;
		exist = loadOBJ(name, vertex, uv, normal);

		indexVBO(vertex, uv, normal, temp.indices, temp.ind_vertex, temp.ind_uv, temp.ind_normal);

		GLuint VertexArrayID;
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		// This will identify our vertex buffer

		glGenBuffers(1, &temp.vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, temp.vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, temp.ind_vertex.size * sizeof(glm::vec3), &temp.ind_vertex.item[0], GL_STATIC_DRAW);

		//color buffer

		glGenBuffers(1, &temp.uvBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, temp.uvBuffer);
		glBufferData(GL_ARRAY_BUFFER, temp.ind_uv.size * sizeof(glm::vec2), &temp.ind_uv.item[0], GL_STATIC_DRAW);

		//normal buffer

		glGenBuffers(1, &temp.normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, temp.normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, temp.ind_normal.size * sizeof(glm::vec3), &temp.ind_normal.item[0], GL_STATIC_DRAW);

		// Generate a buffer for the indices as well

		glGenBuffers(1, &temp.elementBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, temp.elementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, temp.indices.size * sizeof(unsigned short), &temp.indices.item[0], GL_STATIC_DRAW);


		return add(temp);
	}
	else
		printf("not obj file");

	return -1;

}*/

void ParticleManager::setShader(unsigned int particleIndex, unsigned int shaderIndex)
{

	setArray[particleIndex].shaderIndex = shaderIndex;
	return;
}

