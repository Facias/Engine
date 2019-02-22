#include "Sprite.h"
#include <string>

using namespace ENGINE;

int SpriteManager::createSprite(glm::vec2 sz)
{
	Sprite temp;


	temp.shader = &shaderManager->shaderArray[defaultSpriteShaderID];
	temp.shaderIndex = defaultSpriteShaderID;
	temp.texIndex[0] = textureManager->blank;
	temp.texture[0] = textureManager->texArray[temp.texIndex[0]].id;

	temp.setShaderConst("vcolor", 1, 1, 1, 1);

	temp.size = sz;
	temp.setShaderConst("resolution", res.x, res.y, 0, 0);
	temp.setShaderConst("size", sz.x, sz.y, 0, 0);

	glGenBuffers(1, &temp.vertBufferID);

	glBindBuffer(GL_ARRAY_BUFFER, temp.vertBufferID);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(glm::vec2), &vert[0], GL_STATIC_DRAW);

	int n = add(temp);
	setScissor(n, glm::vec2(0), glm::vec2(0));
	setFeather(n, 0.0f);
	setColor(n, glm::vec4(1));

	return n;
}

// Add sprite to manager array
int SpriteManager::add(Sprite item)
{
	if (spriteCount + 2 > arraySize)
	{
		Sprite *temp;
		temp = new Sprite[arraySize * 2];

		for (int n = 0; n < spriteCount; n++)
			temp[n] = spriteArray[n];

		spriteArray = temp;
		arraySize *= 2;

		sortingArray = new Sprite*[arraySize];

		for (int n = 0; n < spriteCount; n++)
			sortingArray[n] = &spriteArray[n];
	}

	spriteArray[spriteCount++] = item;
	sortingArray[spriteCount-1] = &item;
	return spriteCount - 1;
}

void Sprite::updateShaderUniforms()
{
	for (int n = 0; n < constantCount; n++)
	{

		if (constantArray[n].s_type == Shader::S_INT)
		{
			glUniform1i(constantArray[n].location,
				(GLint)constantArray[n].v[0]);
		}
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

}

void Sprite::setShaderConst(char *name, float v0, float v1, float v2, float v3)
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

void SpriteManager::setPosition(int index, glm::vec2 pos)
{
	glm::vec2 offset = (spriteArray[index].size * spriteArray[index].scale) / 2.0f ;
	spriteArray[index].setShaderConst("position", pos.x + offset.x, pos.y + offset.y, 0, 0);
	spriteArray[index].pos = pos;

}

void SpriteManager::setPositionByCenter(int index, glm::vec2 pos)
{
	spriteArray[index].setShaderConst("position", pos.x , pos.y , 0, 0);
	spriteArray[index].pos = pos;

}

void SpriteManager::setColor(int index, glm::vec4 col)
{
	spriteArray[index].setShaderConst("vcolor", col.r, col.g, col.b, col.a);
	spriteArray[index].color = col;

}

void SpriteManager::setScissor(int index, glm::vec2 p0, glm::vec2 p1)
{
	spriteArray[index].setShaderConst("scissor", p0.x, p0.y, p1.x, p1.y);
}

void SpriteManager::setFeather(int index, float feather)
{
	spriteArray[index].setShaderConst("feather", feather,0,0,0);
}

// set the texture array index value and the texture id directly
void SpriteManager::setTexture(int index, int texArrayInd, int texSlot)
{
	spriteArray[index].texIndex[texSlot] = texArrayInd;
	spriteArray[index].texture[texSlot] = textureManager->texArray[texArrayInd].id;
}

void SpriteManager::setVisible(int index, bool state)
{
	spriteArray[index].visible = state;
}

void SpriteManager::setAngle(int index, float angle)
{
	spriteArray[index].angle = angle;

	return;
}

void SpriteManager::setDepth(int index, float depth)
{
	spriteArray[index].depth = depth;

	return;
}

void SpriteManager::setType(int index, char tp)
{
	spriteArray[index].type = tp;
	
	return;
}

void SpriteManager::setSize(int index, glm::vec2 sz)
{
	spriteArray[index].size = sz;
	spriteArray[index].setShaderConst("size", sz.x, sz.y, 0, 0);
	return;
}

glm::vec2 SpriteManager::getPositionByCenter(int index)
{
	return spriteArray[index].pos;
}

glm::vec2 SpriteManager::getPosition(int index)
{
	return spriteArray[index].pos;
}

glm::vec4 SpriteManager::getColor(int index)
{
	return spriteArray[index].color;
}

int SpriteManager::getTexture(int index, int texIndex)
{

	return spriteArray[index].texture[texIndex];
}

bool SpriteManager::getVisible(int index)
{
	return spriteArray[index].visible;
}

float SpriteManager::getDepth(int index)
{
	return spriteArray[index].depth;
}

float SpriteManager::getAngle(int index)
{
	return spriteArray[index].angle;
}

glm::vec2 SpriteManager::getSize(int index)
{
	return spriteArray[index].size;
}

char SpriteManager::getType(int index)
{
	return spriteArray[index].type;
}

void SpriteManager::sort()
{

	if (sortingCount != spriteCount)
	{
		for (int n = 0; n < spriteCount; n++)
		{
			sortingArray[n] = &spriteArray[n];
		}
		sortingCount = spriteCount;
	}
	recQuickSort(0, sortingCount - 1);
}

void SpriteManager::recQuickSort(int left, int right)
{
	if (right - left <= 0)
		return;
	else
	{
		Sprite *pivot = sortingArray[right];


		unsigned int partition = partitionIt(left, right, pivot);
		recQuickSort(left, partition - 1);
		recQuickSort(partition + 1, right);
	}
}

int SpriteManager::partitionIt(int left, int right, Sprite *pivot)
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

void SpriteManager::swap(int dex1, int dex2)
{
	Sprite* temp = sortingArray[dex1];
	sortingArray[dex1] = sortingArray[dex2];
	sortingArray[dex2] = temp;
}