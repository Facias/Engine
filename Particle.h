#ifndef PARTICLE_H_
#define PARTICLE_H_

#include "glm.hpp"

#include <glew.h>
#include "Shader.h"
#include "Stack.h"


namespace ENGINE
{

	struct ParticleSet
	{
		ParticleSet();
		ParticleSet(int size);
		ParticleSet(const ParticleSet &other);

		Stack<unsigned short> indices;
		Stack<glm::vec3> ind_vertex;
		Stack<glm::vec2> ind_uv;
		Stack<glm::vec3> ind_normal;

		GLuint vertexBuffer;
		GLuint uvBuffer;
		GLuint normalBuffer;
		GLuint elementBuffer;


		// individual specs
		glm::vec3 *pos;
		glm::vec3 *rot;
		glm::vec3 *scale;
		glm::vec3 *color;
		GLuint posBufferID;
		GLuint rotBufferID;
		GLuint scaleBufferID;
		GLuint colorBufferID;

		char culling;
		bool visible;
		bool transparent;
		bool castShadow;
		bool follow;
		bool reflectable;

		bool genPlane;
		char blendType;


		unsigned int count;
		short int shaderIndex;


		GLuint texture[4];
		

		ShaderConst constantArray[32];
		char constantCount;

		ShaderMat constantMatArray[16];
		char constantMatCount;

		void setShaderConst(Shader* shader, char *name, float v0, float v1, float v2, float v3);
		void setShaderMat(Shader *shader, char *name, glm::mat4 val);
		void updateShaderUniforms(Shader *shader);
	};

	struct ParticleManager
	{
		ParticleManager()
		{
			setArray = new ParticleSet[8];
			arraySize = 8;
			setCount = 0;
		}

		ParticleSet *setArray;
		short int arraySize;
		short int setCount;

		int add(ParticleSet item);
		//int loadMesh(char *name);
		bool ParticleManager::loadOBJ(
			const char * path,
			Stack < glm::vec3 > & out_vertices,
			Stack < glm::vec2 > & out_uvs,
			Stack < glm::vec3 > & out_normals
			);


		ShaderManager *shaderManager;
		void setShader(unsigned int particleIndex, unsigned int shaderIndex);
	};

}
#endif
