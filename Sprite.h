#ifndef SPRITE_H_
#define SPRITE_H_

#include "glm.hpp"
#include "Shader.h"
#include "Texture.h"

namespace ENGINE
{

	struct Sprite
	{
		Sprite()
		{
			size = glm::vec2(10);
			angle = 0;
			color = glm::vec4(1);
			scale = glm::vec2(1);
			visible = true;
			depth = 10.0;

			shaderIndex = -2;
			constantCount = 0;
		}

		Sprite(const Sprite &other)
		{
			vertBufferID = other.vertBufferID;

			size = other.size;
			pos = other.pos;
			color = other.color;
			scale = other.scale;
			angle = other.angle;
			visible = other.visible;
			depth = other.depth;

			shader = other.shader;
			shaderIndex = other.shaderIndex;
			constantCount = other.constantCount;

			for (int n = 0; n < 4; n++)
				texture[n] = other.texture[n];
			
			for (int n = 0; n < 16; n++)
				constantArray[n] = other.constantArray[n];

		}


		glm::vec2 vert[4];
		glm::vec2 uv[4];
		
		GLuint vertBufferID;

		glm::vec2 size;
		glm::vec2 pos;
		glm::vec2 scale;
		float angle;
		glm::vec4 color;
		bool visible;
		float depth;

		int texture[4];
		Shader *shader;
		unsigned short int shaderIndex;
		ShaderConst constantArray[16];
		char constantCount;
		void setShaderConst(char *name, float val0, float val1, float val2, float val3);
		void updateShaderUniforms();

	};

	struct SpriteManager
	{
		SpriteManager()
		{
			uv[0] = glm::vec2(0,1);
			uv[1] = glm::vec2(1,1);
			uv[2] = glm::vec2(1,0);
			uv[3] = glm::vec2(0,1);
			uv[4] = glm::vec2(1,0);
			uv[5] = glm::vec2(0,0);

			glGenBuffers(1, &uvBufferID);
			glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
			glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(glm::vec2), &uv[0], GL_STATIC_DRAW);

			spriteArray = new Sprite[16];
			sortingArray = new Sprite*[16];
			arraySize = 16;
			spriteCount = 0;
		}
		Sprite *spriteArray;

		unsigned short int spriteCount;
		unsigned short int arraySize;

		glm::vec2 uv[6];
		GLuint uvBufferID;

		int createSprite(glm::vec2 size);

		int add(Sprite item);

		unsigned short int defaultSpriteShaderID;

		void setPositionByCenter(int index, glm::vec2 pos);
		void setPosition(int index, glm::vec2 pos);
		void setColor(int index, glm::vec4 col);
		void setTexture(int index, int texID, int texIndex);
		void setVisible(int index, bool state);
		void setAngle(int index, float ang);
		void setSize(int index, glm::vec2 sz);
		void setDepth(int index, float depth);
		void setScissor(int index, glm::vec2 p0, glm::vec2 p1);
		void setFeather(int index, float feather);

		glm::vec2 getPositionByCenter(int index);
		glm::vec2 getPosition(int index);
		glm::vec4 getColor(int index);
		int getTexture(int index, int texIndex);
		bool getVisible(int index);
		float getAngle(int index);
		glm::vec2 getSize(int index);
		float getDepth(int index);

		void sort();
		void recQuickSort(int left, int right);
		int partitionIt(int left, int right, Sprite *pivot);
		void swap(int dex1, int dex2);

		Sprite **sortingArray;
		int sortingCount;


		ShaderManager *shaderManager;
		TextureManager *textureManager;
	};
}
#endif
