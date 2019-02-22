
#include <cstring>

#include <glew.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <fstream>
#include <iostream>

#include "Texture.h"

#include "Text.h"

using namespace ENGINE;


/*
void Text::cleanupText2D() {

	// Delete buffers
	glDeleteBuffers(1, &Text2DVertexBufferID);
	glDeleteBuffers(1, &Text2DUVBufferID);

	// Delete texture
	//glDeleteTextures(1, FontTexture.id);

	// Delete shader
	glDeleteProgram(Text2DShaderID);
}*/

TextObj::TextObj()
{
	type = TextManager::STANDARD;
	//vertices = new glm::vec2[256];
	//UVs = new glm::vec2[256];
	//str = new char[256];
	size = 36.0f;
	constantCount = 0;
	visible = true;
	color = glm::vec4(1);
	depth = 9.9f;
}

TextManager::TextManager()
{
	textArray = new TextObj[16];
	textCount = 0;
	textArraySize = 16;

	fontArray = new Font[4];
	fontCount = 0;
	fontArraySize = 4;

	//sortingArray = new TextObj*[16];
}

void TextManager::init()
{
	// Initialize VBO
	glGenBuffers(1, &Text2DVertexBufferID);
	glGenBuffers(1, &Text2DUVBufferID);

	// Initialize Shader
	textShaderIndex = shaderManager->loadShader("TextVertexShader.vs", "textShader.ps");
	textShaderID = shaderManager->shaderArray[textShaderIndex].id;
	shader = &shaderManager->shaderArray[textShaderIndex];

	// Initialize uniforms' IDs
	Text2DUniformID = glGetUniformLocation(textShaderID, "texture0");
}

int TextManager::createText(char *str)
{
	TextObj temp;
	strcpy(temp.str, str);
	temp.pos = glm::vec2(100, 100);
	temp.fontID = 0;

	int n = addText(temp);
	setColor(n, glm::vec4(1));
	setScissor(n, glm::vec2(0), glm::vec2(0));
	setFeather(n, 0.0f);

	return n;
}

int TextManager::createText(char *str, glm::vec2 pos, float size)
{
	TextObj temp;
	strcpy(temp.str , str);
	temp.size = size;
	temp.pos = pos;
	temp.fontID = 0;

	int n = addText(temp);
	setColor(n, glm::vec4(1));
	setScissor(n, glm::vec2(0), glm::vec2(0));
	setFeather(n, 0.0f);

	return n;
}

int TextManager::addText(TextObj item)
{
	if (textCount + 2 > textArraySize)
	{
		TextObj *temp = new TextObj[2 * textArraySize];
		for (int n = 0; n < textCount; n++)
		{
			temp[n] = textArray[n];
		}

		textArraySize *= 2;
		delete[] textArray;
		textArray = temp;

		//sortingArray = new TextObj*[textArraySize];
	}

	textArray[textCount++] = item;

	return textCount - 1;
}

int TextManager::addFont(Font item)
{
	if (fontCount + 2 > fontArraySize)
	{
		Font *temp = new Font[2 * fontArraySize];
		for (int n = 0; n < fontCount; n++)
		{
			temp[n] = fontArray[n];
		}

		fontArraySize *= 2;
		delete[] fontArray;
		fontArray = temp;
	}

	fontArray[fontCount++] = item;

	return fontCount - 1;
}

int TextManager::loadFont(char *filename)
{
	Font temp;
	// Initialize texture
	int a = textureManager->loadImage(filename);
	temp.fontImg = textureManager->texArray[a];
	printf("\n\n\nfont img index %d \n\n\n\n", a);


	// load font layout info
	char *name;
	char sub[256];
	strcpy(sub, filename);
	name = strtok(sub, ".");
	strcat(name, " subimages.txt");


	int index, xpos, ypos, width, height;

	FILE *subfile = fopen(name, "r");
	if (!subfile)
	{
		printf("font subimage %s could not be opened", name);
		return -2;
	}
	else
	{
		while (1)
		{
			int res;

			res = fscanf(subfile, "%d:%d:%d:%d:%d", &index, &xpos, &ypos, &width, &height);
			if (res == EOF) break;
			temp.characterPos[index] = glm::vec2(xpos, ypos);
			temp.characterSize[index] = glm::vec2(width, height);
		}
	}
	fclose(subfile);

	return addFont(temp);
}

void TextManager::setString(int index, char *str)
{

	strcpy(textArray[index].str ,str);
}

char *TextManager::getString(int index)
{
	return textArray[index].str;
}

/*
void TextManager::setString(int index, const char *str)
{
	textArray[index].str = new char[strlen(str)];
	//strcpy(textArray[index].str, str);
}
*/

void TextManager::setType(int index, char tp)
{
	textArray[index].type = tp;
}

void TextManager::setVisible(int index, bool state)
{
	textArray[index].visible = state;
}

void TextManager::setColor(int index, glm::vec4 col)
{
	textArray[index].color = col;
	setShaderConst(index, "vcolor", col.r, col.g, col.b, col.a);
}

void TextManager::setSize(int index, float sz)
{
	textArray[index].size = sz;
}

void TextManager::setPosition(int index, glm::vec2 pos)
{
	textArray[index].pos = pos;
}

void TextManager::setScissor(int index, glm::vec2 p0, glm::vec2 p1)
{
	setShaderConst(index, "scissor", p0.x, p0.y, p1.x, p1.y);
}

void TextManager::setFeather(int index, float feather)
{
	setShaderConst(index, "feather",feather, 0,0,0);
}

void TextManager::draw(int index)
{
	TextObj *current = &textArray[index];
	Font *currentFont = &fontArray[textArray[index].fontID];
	length = strnlen_s(current->str,20);

	float xpos = current->pos.x;
	float ypos = current->pos.y;
	float scale = current->size / fontArray[current->fontID].characterSize[current->str[0]].y;


	// Fill buffers
	for (unsigned int i = 0; i<length; i++) 
	{

		vertex_up_left = glm::vec2(xpos, ypos + current->size);
		vertex_up_right = glm::vec2(xpos + currentFont->characterSize[current->str[i]].x *scale, ypos + current->size);
		vertex_down_right = glm::vec2(xpos + currentFont->characterSize[current->str[i]].x*scale, ypos);
		vertex_down_left = glm::vec2(xpos, ypos);

		xpos += currentFont->characterSize[current->str[i]].x*scale;

		vertices[i * 6] = vertex_up_left;

		vertices[i * 6 + 1] = vertex_down_left;
		vertices[i * 6 + 2] = vertex_up_right;
		vertices[i * 6 + 3] = vertex_down_right;
		vertices[i * 6 + 4] = vertex_up_right;
		vertices[i * 6 + 5] = vertex_down_left;

		character = current->str[i];

		glm::vec2 pos;
		glm::vec2 csize;
		pos = currentFont->characterPos[character];
		csize = currentFont->characterSize[character];

		glm::vec2 fontImgSize = currentFont->fontImg.size;
		uv_up_left = glm::vec2(pos.x, pos.y + csize.y) / float(fontImgSize.x);
		uv_up_right = glm::vec2(pos.x + csize.x, pos.y + csize.y) / float(fontImgSize.x);
		uv_down_right = glm::vec2(pos.x + csize.x, pos.y) / float(fontImgSize.y);
		uv_down_left = glm::vec2(pos.x, pos.y) / float(fontImgSize.y);

		
		UVs[i * 6] = uv_up_left;
		UVs[i * 6 + 1] = uv_down_left;
		UVs[i * 6 + 2] = uv_up_right;

		UVs[i * 6 + 3] = uv_down_right;
		UVs[i * 6 + 4] = uv_up_right;
		UVs[i * 6 + 5] = uv_down_left;
	}

	glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, length * 6 * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
	glBufferData(GL_ARRAY_BUFFER, length * 6 * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);

	// Bind shader
	glUseProgram(textShaderID);

	updateShaderUniforms(index);

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, currentFont->fontImg.id);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(Text2DUniformID, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnable(GL_BLEND);
	glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
	glDisable(GL_SAMPLE_ALPHA_TO_ONE);
	glDepthFunc(GL_ALWAYS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw call

	glDrawArrays(GL_TRIANGLES, 0, length * 6);

	glDepthFunc(GL_LESS);
	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

}

void TextManager::updateShaderUniforms(int index)
{
	TextObj *text = &textArray[index];

	for (int n = 0; n < text->constantCount; n++)
	{

		if (text->constantArray[n].s_type == Shader::S_INT)
		{
			glUniform1i(text->constantArray[n].location,
				(GLint)text->constantArray[n].v[0]);
		}
		if (text->constantArray[n].s_type == Shader::S_FLOAT)
		{
			glUniform1f(text->constantArray[n].location,
				text->constantArray[n].v[0]);
		}
		if (text->constantArray[n].s_type == Shader::S_VEC2)
		{
			glUniform2f(text->constantArray[n].location,
				text->constantArray[n].v[0],
				text->constantArray[n].v[1]);
		}
		if (text->constantArray[n].s_type == Shader::S_VEC3)
		{
			glUniform3f(text->constantArray[n].location,
				text->constantArray[n].v[0],
				text->constantArray[n].v[1],
				text->constantArray[n].v[2]);
		}
		if (text->constantArray[n].s_type == Shader::S_VEC4)
		{
			glUniform4f(text->constantArray[n].location,
				text->constantArray[n].v[0],
				text->constantArray[n].v[1],
				text->constantArray[n].v[2],
				text->constantArray[n].v[3]);
		}
	}

}

void TextManager::setShaderConst(int index, char *name, float v0, float v1, float v2, float v3)
{
	GLint loc;
	unsigned char type;
	

	loc = shader->getUniformLocation(name);
	type = shader->getUniformType(name);

	TextObj *text = &textArray[index];
	
	// check if uniform name already exists on this object
	for (int n = 0; n < text->constantCount; n++)
	{
		if (text->constantArray[n].name != NULL)
		{
			if (strcmp(name, text->constantArray[n].name) == 0)
			{
				text->constantArray[n].v[0] = v0;
				text->constantArray[n].v[1] = v1;
				text->constantArray[n].v[2] = v2;
				text->constantArray[n].v[3] = v3;
				text->constantArray[n].location = loc;
				text->constantArray[n].s_type = type;

				return;
			}
		}
	}

	// if uniform name does not already exist... add it
	text->constantArray[text->constantCount].name = name;
	text->constantArray[text->constantCount].v[0] = v0;
	text->constantArray[text->constantCount].v[1] = v1;
	text->constantArray[text->constantCount].v[2] = v2;
	text->constantArray[text->constantCount].v[3] = v3;
	text->constantArray[text->constantCount].location = loc;
	text->constantArray[text->constantCount].s_type = type;

	text->constantCount++;
	
	return;
}

char TextManager::getType(int index)
{
	return textArray[index].type;
}

bool TextManager::getVisible(int index)
{
	return textArray[index].visible;
}

glm::vec4 TextManager::getColor(int index)
{
	return textArray[index].color;
}

float TextManager::getSize(int index)
{
	return textArray[index].size;
}

glm::vec2 TextManager::getPosition(int index)
{
	return textArray[index].pos;
}
/*
void TextManager::sort()
{

	if (sortingCount != textCount)
	{
		for (int n = 0; n < textCount; n++)
		{
			sortingArray[n] = &textArray[n];
		}
		sortingCount = textCount;
	}
	recQuickSort(0, sortingCount - 1);
}

void TextManager::recQuickSort(int left, int right)
{
	if (right - left <= 0)
		return;
	else
	{
		TextObj *pivot = sortingArray[right];


		unsigned int partition = partitionIt(left, right, pivot);
		recQuickSort(left, partition - 1);
		recQuickSort(partition + 1, right);
	}
}

int TextManager::partitionIt(int left, int right, TextObj *pivot)
{
	int leftPtr = left - 1;
	int rightPtr = right;
	while (true)
	{
		while (sortingArray[++leftPtr]->depth > pivot->depth)
			;
		while (rightPtr > 0 && sortingArray[--rightPtr]->depth < pivot->depth)
			;
		if (leftPtr >= rightPtr)
			break;
		else
			swap(leftPtr, rightPtr);
	}
	swap(leftPtr, right);
	return leftPtr;
}

void TextManager::swap(int dex1, int dex2)
{
	TextObj* temp = sortingArray[dex1];
	sortingArray[dex1] = sortingArray[dex2];
	sortingArray[dex2] = temp;
}*/

void TextManager::setDepth(int index, float depth)
{
	textArray[index].depth = depth;
}

float TextManager::getDepth(int index)
{
	return textArray[index].depth;
}