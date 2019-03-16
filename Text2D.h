#ifndef TEXT2D_H_
#define TEXT2D_H_

#define _CRT_SECURE_NO_WARNINGS

#include "glm.hpp"
#include "Texture.h"
#include "Shader.h"

namespace ENGINE
{

	struct Font
	{
		Font()
		{

		}

		Font(const Font &other)
		{
			fontImg = other.fontImg;
			for (int n = 0; n < 128; n++)
			{
				characterPos[n] = other.characterPos[n];
				characterSize[n] = other.characterSize[n];
			}
		}

		TextureObj fontImg;
		glm::vec2 characterPos[128];
		glm::vec2 characterSize[128];
	};


	struct TextObj
	{
		TextObj()
		{
			//vertices = new glm::vec2[256];
			//UVs = new glm::vec2[256];
			//str = new char[256];
			size = 36.0f;
			constantCount = 0;
			visible = true;
			color = glm::vec4(1);
			depth = 9.9f;
		}
		TextObj(const TextObj &other)
		{
			//char temp[1024];
			//strcpy(temp, other.str);
			strcpy(str, other.str);
			//str = other.str;
			pos = other.pos;
			size = other.size;
			color = other.color;
			fontID = other.fontID;
			constantCount = other.constantCount;
			visible = other.visible;
			depth = other.depth;
		}

		char str[256];
		glm::vec2 pos;
		glm::vec4 color;
		float size;
		bool visible;
		float depth;

		int fontID;

		ShaderConst constantArray[16];
		char constantCount;

	};

	struct TextManager
	{
		TextManager();
		void init();

		TextObj *textArray;
		unsigned short int textCount;
		unsigned short int textArraySize;

		Font *fontArray;
		unsigned char fontCount;
		unsigned char fontArraySize;

		int createText(char *str);
		int createText(char *str, glm::vec2 pos, float size);
		int loadFont(char *filename);
		void setString(int index, char *str);
		//void setString(int index, const char *str);
		void setVisible(int index, bool state);
		void setColor(int index, glm::vec4 col);
		void setSize(int index, float sz);
		void setPosition(int index, glm::vec2 pos);
		void setDepth(int index, float depth);
		void setScissor(int index, glm::vec2 p0, glm::vec2 p1);
		void setFeather(int index, float feather);

		bool getVisible(int index);
		glm::vec4 getColor(int index);
		float getSize(int index);
		glm::vec2 getPosition(int index);
		float getDepth(int index);
		char *getString(int index);
		
		void draw(int index);

		int addFont(Font item);
		int addText(TextObj item);

		GLint textShaderID;

		glm::vec2 vertices[256];
		glm::vec2 UVs[256];
		char character;

		glm::vec2 vertex_up_left;
		glm::vec2 vertex_up_right;
		glm::vec2 vertex_down_right;
		glm::vec2 vertex_down_left;

		glm::vec2 uv_up_left;
		glm::vec2 uv_up_right;
		glm::vec2 uv_down_right;
		glm::vec2 uv_down_left;
		int length;

		unsigned int Text2DVertexBufferID;
		unsigned int Text2DUVBufferID;
		unsigned int Text2DUniformID;

		TextureManager *textureManager;
		ShaderManager *shaderManager;
		Shader *shader;
		int textShaderIndex;
		/*
		void sort();
		void recQuickSort(int left, int right);
		int partitionIt(int left, int right, TextObj *pivot);
		void swap(int dex1, int dex2);

		TextObj **sortingArray;
		int sortingCount;
		*/
		void updateShaderUniforms(int index);
		void setShaderConst(int index, char *name, float v0, float v1, float v2, float v3);
	};

}
#endif