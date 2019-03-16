#ifndef TEXTURE_H_
#define TEXTURE_H_

#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <glew.h>
#include "Stack.h"
#include "glm.hpp"


namespace ENGINE
{

	struct TextureObj
	{
		TextureObj();
		TextureObj(const TextureObj &other);
		
		GLint id;
		char name[128];
		bool wrapU;
		bool wrapV;
		bool magFilter;
		bool minFilter;
		bool mipmap;
		bool mipmapCreated;
		float anisotropy;
		glm::vec2 size;

		bool exists;
	};

	struct RenderTexture
	{
		RenderTexture()
		{
			fbo = 0;
			color = 5;
			depth = 60;
			size = glm::ivec2(1);
			mipmap = false;
		}

		RenderTexture(const RenderTexture &other)
		{
			fbo = other.fbo;
			color = other.color;
			depth = other.depth;
			colorID = other.colorID;
			depthID = other.depthID;
			size = other.size;
			mipmap = other.mipmap;
		}

		GLuint fbo;
		GLuint colorID;
		GLuint depthID;

		int color;
		int depth;
		
		glm::ivec2 size;
		bool mipmap;
	};

	struct GBuffer
	{
		GBuffer()
		{
			fbo = 0;
			position = 5;
			normal = 6;
			color = 7;
			depth = 60;
			size = glm::ivec2(1);
			mipmap = false;
		}

		GBuffer(const GBuffer &other)
		{
			fbo = other.fbo;
			position = other.position;
			normal = other.normal;
			color = other.color;
			depth = other.depth;
			positionID = other.positionID;
			normalID = other.normalID;
			colorID = other.colorID;
			depthID = other.depthID;
			size = other.size;
			mipmap = other.mipmap;
		}

		GLuint fbo;
		GLuint positionID;
		GLuint normalID;
		GLuint colorID;
		GLuint depthID;

		int position;
		int normal;
		int color;
		int depth;

		glm::ivec2 size;
		bool mipmap;
	};

	struct TextureManager
	{
		TextureManager()
		{
		
			blank = loadImage("white.png");
		}

		Stack<TextureObj> texArray;

		int loadImage(char *name);
		void deleteImage(int index);
		void deleteImages(int *index, int count);

		void setWrap(int index, bool u, bool v);


		GLint loadDDS(const char *imagepath);
		GLint loadBMP_custom(const char *imagepath);
		RenderTexture createRenderTexture(int width, int height, bool mipmapping);
		RenderTexture createRenderTextureMultisample(int width, int height, int samples);
		GBuffer createGBuffer(int width, int height);

		// default, white texture for standard asset creation
		int blank;
	};
}




#endif