#ifndef SHADOW_MAP_H_
#define SHADOW_MAP_H_

#include "Texture.h"

namespace ENGINE
{

	struct ShadowMap
	{
		ShadowMap()
		{
			res = 1024;

		}

		void init(TextureManager *textureManager)
		{
			map[0] = textureManager->createRenderTexture(res, res, false);
			map[1] = textureManager->createRenderTexture(res, res, false);
			map[2] = textureManager->createRenderTexture(res, res, false);
			map[3] = textureManager->createRenderTexture(res, res, false);
			compMap = textureManager->createRenderTexture(res * 4, res, false);

			glBindTexture(GL_TEXTURE_2D, compMap.colorID);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, GLint(0));

			glBindTexture(GL_TEXTURE_2D, compMap.depthID);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, GLint(0));
		}

		RenderTexture map[4];
		RenderTexture compMap;

		glm::mat4 orthoProj[4];
		glm::mat4 camView[4];
		glm::mat4 viewProj[4];
		glm::mat4 mvp_shadow;
		glm::vec3 cascadeCenter[4];

		float cascadeRadius[4]; // radius of cascade sphere
		float cascadeCenterDist[4]; // distance from camera to cascade center

		short int res;
		char cascadeCount;


	};


}

#endif