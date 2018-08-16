#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glew.h>

//#include <glfw3.h>
#include "Texture.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

using namespace ENGINE;

TextureObj::TextureObj()
{
	id = -1;
	wrapU = true;
	wrapV = true;
	magFilter = true;
	minFilter = true;
	mipmap = false;
	mipmapCreated = false;
	anisotropy = 0.0f;
	size = glm::vec2(0);
}

TextureObj::TextureObj(const TextureObj &other)
{
	id = other.id;
	strcpy(name, other.name);
	wrapU = other.wrapU;
	wrapV = other.wrapV;
	magFilter = other.magFilter;
	minFilter = other.minFilter;
	mipmap = other.mipmap;
	mipmapCreated = other.mipmapCreated;
	anisotropy = other.anisotropy;
	size = other.size;
}

GLint TextureManager::loadBMP_custom(const char * imagepath)
{

	printf("Reading image %s\n", imagepath);

	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	// Actual RGB data
	unsigned char * data;

	// Open the file
	FILE * file = NULL;
	fopen_s(&file, imagepath, "rb");
	if (!file) {
		printf("%s could not be opened.\n", imagepath);
		getchar();
		return 0;
	}

	// Read the header, i.e. the 54 first bytes

	// If less than 54 bytes are read, problem
	if (fread(header, 1, 54, file) != 54) 
	{
		printf("Not a correct BMP file\n");
		fclose(file);
		return 0;
	}

	// A BMP files always begins with "BM"
	if (header[0] != 'B' || header[1] != 'M')
	{
		printf("Not a correct BMP file\n");
		fclose(file);
		return 0;
	}

	// Make sure this is a 24bpp file
	if (*(int*)&(header[0x1E]) != 0) 
	{
		printf("Not a correct BMP file\n");
		fclose(file); return 0; 
	}
	if (*(int*)&(header[0x1C]) != 24)
	{ 
		printf("Not a correct BMP file\n");    
		fclose(file); return 0; 
	}

	// Read the information about the image
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0) imageSize = width*height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0) dataPos = 54; // The BMP header is done that way

										 // Create a buffer
	data = new unsigned char[imageSize];

	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);

	// Everything is in memory now, the file can be closed.
	fclose(file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	// OpenGL has now copied the data. Free our own version
	delete[] data;

	// Poor filtering, or ...
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

	// ... nice trilinear filtering ...
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);

	// ... which requires mipmaps. Generate them automatically.
	glGenerateMipmap(GL_TEXTURE_2D);

	// Return the ID of the texture we just created
	return textureID;
}


#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

GLint TextureManager::loadDDS(const char * imagepath) 
{

	unsigned char header[124];

	FILE *fp = NULL;

	/* try to open the file */
	fopen_s(&fp, imagepath, "rb");
	if (fp == NULL) 
	{
		printf("%s could not be opened.\n", imagepath); getchar();
		return 0;
	}

	/* verify the type of file */
	char filecode[4];
	fread(filecode, 1, 4, fp);
	if (strncmp(filecode, "DDS ", 4) != 0) 
	{
		fclose(fp);
		return 0;
	}

	/* get the surface desc */
	fread(&header, 124, 1, fp);

	unsigned int height = *(unsigned int*)&(header[8]);
	unsigned int width = *(unsigned int*)&(header[12]);
	unsigned int linearSize = *(unsigned int*)&(header[16]);
	unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC = *(unsigned int*)&(header[80]);


	unsigned char * buffer;
	unsigned int bufsize;
	/* how big is it going to be including all mipmaps? */
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
	buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
	fread(buffer, 1, bufsize, fp);
	/* close the file pointer */
	fclose(fp);

	unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
	unsigned int format;
	switch (fourCC)
	{
	case FOURCC_DXT1:
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case FOURCC_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case FOURCC_DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		free(buffer);
		return 0;
	}

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;

	/* load the mipmaps */
	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
	{
		unsigned int size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
			0, size, buffer + offset);

		offset += size;
		width /= 2;
		height /= 2;

		// Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
		if (width < 1) width = 1;
		if (height < 1) height = 1;

	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);

	free(buffer);

	return textureID;


}

int TextureManager::loadImage(char *name)
{
	TextureObj temp;


	char * ext;
	char filename[1024];
	strcpy(filename, name);

	ext = strtok(filename, ".");
	ext = strtok(NULL, ".");
	bool successful = false;

	if (strcmp(ext, "bmp") == 0)
	{
		temp.id = loadBMP_custom(name);
		strcpy(temp.name, name);
		successful = true;
		
	}

	if (strcmp(ext, "DDS") == 0)
	{
		temp.id = loadDDS(name);
		strcpy(temp.name, name);
		successful = true;
	}

	if (strcmp(ext, "png") == 0)
	{
		glm::ivec2 size;
		int components;
		unsigned char *data;
		data = stbi_load(name, &size.x, &size.y, &components, 4);

		GLuint textureID;
		glGenTextures(1, &textureID);

		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
		temp.size = size;
		strcpy(temp.name, name);
		temp.id = textureID;

		successful = true;
	}

	if (!successful)
	{
		printf("could not load image %s. \n", name);
		return -2;
	}
	else
	{
		printf("image %s successful\n", name);
		return add(temp);
	}
}

int TextureManager::add(TextureObj item)
{
	if (texCount + 2 > arraySize)
	{
		TextureObj *temp = new TextureObj[2*arraySize];

		for (int n = 0; n < texCount; n++)
		{
			temp[n] = texArray[n];
		}

		arraySize *= 2;
		delete[] texArray;
		texArray = temp;
	}

	texArray[texCount++] = item;

	return texCount - 1;
}

void TextureManager::setWrap(int index, bool u, bool v)
{
	glBindTexture(GL_TEXTURE_2D, texArray[index].id);
	if (u)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);

	if (v)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	texArray[index].wrapU = u;
	texArray[index].wrapV = v;
}

RenderTexture TextureManager::createRenderTexture(int width, int height, bool mipmapping)
{
	RenderTexture temp;

	glGenFramebuffers(1, &temp.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, temp.fbo);

	glGenTextures(1, &temp.colorID);

	glBindTexture(GL_TEXTURE_2D, temp.colorID);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, temp.colorID, 0);

	if (mipmapping)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
		//for (int n = 1; n < 8; n++)
		{
			//glTexImage2D(GL_TEXTURE_2D, n, GL_RGB, width / pow(2,n), height / pow(2,n), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, temp.color, n);
		}
	}

	if (mipmapping)
	{
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		temp.mipmap = true;
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		temp.mipmap = false;
	}

	temp.size = glm::ivec2(width, height);

	glGenTextures(1, &temp.depthID);
	glBindTexture(GL_TEXTURE_2D, temp.depthID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, temp.depthID, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Set the list of draw buffers.
	//GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	//glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) printf("error creating render buffers");

	TextureObj tempColor;
	TextureObj tempDepth;
	tempColor.id = temp.colorID;
	tempColor.size = temp.size;
	tempDepth.id = temp.depthID;
	tempDepth.size = temp.size;
	
	temp.color = add(tempColor);
	temp.depth = add(tempDepth);
	return temp;

}

RenderTexture TextureManager::createRenderTextureMultisample(int width, int height, int samples)
{
	RenderTexture temp;

	//GLuint atex;
	//glGenTextures(1, &atex);
	//glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, atex);
	//glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGBA, width, height, GL_FALSE);

	glGenFramebuffers(1, &temp.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, temp.fbo);


	glGenTextures(1, &temp.colorID);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, temp.colorID);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA, width, height, GL_FALSE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, temp.colorID, 0);

	glGenTextures(1, &temp.depthID);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, temp.depthID);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_DEPTH_COMPONENT, width, height, GL_FALSE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, temp.depthID, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) printf("error creating render buffers");


	//glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	/*
	glGenRenderbuffers(1, &temp.color);
	glBindRenderbuffer(GL_RENDERBUFFER, temp.color);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGBA, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, temp.color);
	*/

	temp.size = glm::ivec2(width, height);
	/*
	// Create depth render buffer (This is optional)
	glGenRenderbuffers(1, &temp.depth);
	glBindRenderbuffer(GL_RENDERBUFFER, temp.depth);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT24, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, temp.depth);
	*/
	// Set "renderedTexture" as our colour attachement #0
	//
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, temp.depth, 0);


	// Set the list of draw buffers.
	//GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	//glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	TextureObj tempColor;
	TextureObj tempDepth;
	tempColor.id = temp.colorID;
	tempColor.size = temp.size;
	tempDepth.id = temp.depthID;
	tempDepth.size = temp.size;

	temp.color = add(tempColor);
	temp.depth = add(tempDepth);

	
	return temp;

}

/*
RenderTexture createDepthTexture(int width, int height)
{
	RenderTexture depthrenderbuffer;
	glGenTextures(1, &depthrenderbuffer.depthID);
	glBindTexture(GL_TEXTURE_2D, depthrenderbuffer.depthID);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// The depth buffer

	glGenRenderbuffers(1, &depthrenderbuffer.fbo);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer.fbo);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthrenderbuffer.depth, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	return depthrenderbuffer;
}
*/

// create writable G buffer for deferred rendering
GBuffer TextureManager::createGBuffer(int width, int height)
{
	GBuffer temp;

	unsigned int gBuffer;
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	//unsigned int gPosition, gNormal, gColorSpec;

	// - position color buffer
	glGenTextures(1, &temp.positionID);
	glBindTexture(GL_TEXTURE_2D, temp.positionID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, temp.positionID, 0);

	// - normal color buffer
	glGenTextures(1, &temp.normalID);
	glBindTexture(GL_TEXTURE_2D, temp.normal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, temp.normalID, 0);

	// - color + specular color buffer
	glGenTextures(1, &temp.colorID);
	glBindTexture(GL_TEXTURE_2D, temp.colorID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, temp.colorID, 0);

	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	temp.size = glm::ivec2(width, height);

	glGenTextures(1, &temp.depthID);
	glBindTexture(GL_TEXTURE_2D, temp.depthID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, temp.depthID, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Set the list of draw buffers.
	//GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	//glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) printf("error creating render buffers");

	TextureObj tempPosition;
	TextureObj tempNormal;
	TextureObj tempColor;
	TextureObj tempDepth;
	tempPosition.id = temp.positionID;
	tempPosition.size = temp.size;
	tempNormal.id = temp.normalID;
	tempNormal.size = temp.size;
	tempColor.id = temp.colorID;
	tempColor.size = temp.size;
	tempDepth.id = temp.depthID;
	tempDepth.size = temp.size;

	temp.position = add(tempPosition);
	temp.normal = add(tempNormal);
	temp.color = add(tempColor);
	temp.depth = add(tempDepth);
	return temp;

}