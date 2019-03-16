#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef SCENE_SCRIPT_H_
#define SCENE_SCRIPT_H_

#include "glm.hpp"



namespace ENGINE
{

	struct EntityEntry
	{
		EntityEntry();
		EntityEntry(const EntityEntry &other);
		bool operator ==(EntityEntry &other);
		
		char structureType;
		char meshName[256];
		bool collision;
		bool culling;
		bool transparent;
		bool visible;
		bool castShadow;
		bool follow;
		bool lookatCam;
		bool lookatPosState;
		bool reflectable;
		bool depthMap; // assign depth map to mesh
		bool opaqueMap; // assign opaque map for distortion
		bool reflectMap;
		bool isWater;
		glm::vec3 lookatPos;
		
		// for generic plane mesh
		bool genPlane;
		glm::vec2 planeSize;
		
		glm::vec3 pos;
		glm::vec3 rot;
		glm::vec3 scale;

		char vertShader[1024];
		char fragShader[1024];
		char uniformName[256][32];
		float uniformVal[4][32];
		char uniformCount;
		bool shaderAssigned;

		char texName[16][256];
		bool texWrap[16];
		bool texAssigned[16];
	};

	struct SceneScript
	{
		SceneScript()
		{
			entityCount = 0;
			entityArray = new EntityEntry[64];
			arraySize = 64;
			skipping = false;
		}
		void parseFile(char *name);

		bool skipping;
		int entityCount;
		EntityEntry * entityArray;
		int arraySize;

		glm::vec3 sunpos;
	};
}

#endif