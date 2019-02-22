#include "SceneScript.h"

#include <windows.h>
#include <string>
#include <fstream>

using namespace ENGINE;

EntityEntry::EntityEntry()
{
	structureType = 0;
	collision = 1;
	culling = 1;
	transparent = false;
	visible = 1;
	castShadow = 1;
	follow = false;
	lookatCam = false;
	lookatPosState = false;
	depthMap = false;
	opaqueMap = false;
	reflectMap = false;
	isWater = false;
	lookatPos = glm::vec3(0);

	genPlane = false;

	pos = glm::vec3(0);
	rot = glm::vec3(0);
	scale = glm::vec3(1);

	uniformCount = 0;
	shaderAssigned = false;

	for (int n = 0; n < 16; n++)
	{
		texAssigned[n] = 0;
		texWrap[n] = 1;
	}
}

EntityEntry::EntityEntry(const EntityEntry &other)
{
	structureType = other.structureType;
	strcpy(meshName, other.meshName);
	collision = other.collision;
	culling = other.culling;
	transparent = other.transparent;
	visible = other.visible;
	castShadow = other.castShadow;
	follow = other.follow;
	lookatCam = other.lookatCam;
	lookatPosState = other.lookatPosState;
	depthMap = other.depthMap;
	opaqueMap = other.opaqueMap;
	reflectMap = other.reflectMap;
	isWater = other.isWater;
	lookatPos = other.lookatPos;

	genPlane = other.genPlane;
	planeSize = other.planeSize;

	strcpy(vertShader, other.vertShader);
	strcpy(fragShader, other.fragShader);
	uniformCount = other.uniformCount;
	shaderAssigned = other.shaderAssigned;

	pos = other.pos;
	rot = other.rot;
	scale = other.scale;


	for (int n = 0; n < 32; n++)
	{
		if (n <= uniformCount)
		{
			strcpy(uniformName[n], other.uniformName[n]);
			uniformVal[0][n] = other.uniformVal[0][n];
			uniformVal[1][n] = other.uniformVal[1][n];
			uniformVal[2][n] = other.uniformVal[2][n];
			uniformVal[3][n] = other.uniformVal[3][n];
		}
	}

	for (int n = 0; n < 16; n++)
	{
		texAssigned[n] = other.texAssigned[n];
		texWrap[n] = other.texWrap[n];
		strcpy(texName[n], other.texName[n]);
	}


}

void SceneScript::parseFile(char *name)
{
	
	FILE *file = NULL;

	fopen_s(&file, name, "r");
	if (file == NULL)
	{
		printf("script file %s cannot be opened", name);
		return;
	}
	printf("\n\n");
	while (1)
	{
		char line[1024];
		char piece[16][512]; // if memory problem, swap ranges

		char *getLine = fgets(line, sizeof(line), file);
		if (getLine == NULL)
		{
			printf("finished reading scene script %s.", name);
			break;
		}
		int pieceCount = 0;
		pieceCount = sscanf(line, "%s %[^' ,\n']%[^' ,\n']%[^' ,\n']%[^' ,\n']", &piece[0], &piece[1], &piece[2], &piece[3], &piece[4]);

		if (strcmp("/*", piece[0]) == 0)
		{

			skipping = true;
		}

		if (strcmp("*/", piece[0]) == 0)
		{
			skipping = false;
		}

		if (!skipping)
		{
			printf("in\n");
			if (strcmp("define", piece[0]) == 0)
			{
				pieceCount = sscanf(line, "%s %s", &piece[0], &piece[1]);
				if (strcmp(piece[1], "particle") == 0)
				{
					entityArray[entityCount - 1].structureType = 1;
				}
				entityCount++;
			}

			if (strcmp("meshName", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					strcpy(entityArray[entityCount - 1].meshName, piece[1]);
				}
			}

			if (strcmp("meshPlane", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					int cc = sscanf(line, "%s %[^' ,'],%s", &piece[0], &piece[1], &piece[2]);
					float val0 = std::stof(piece[1]);
					float val1 = std::stof(piece[2]);
					strcpy(entityArray[entityCount - 1].meshName, "gen_plane");
					entityArray[entityCount - 1].planeSize = glm::vec2(val0, val1);
					entityArray[entityCount - 1].genPlane = true;
				}
			}

			if (strcmp("collision", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					int val = std::stoi(piece[1]);
					entityArray[entityCount - 1].collision = val;

				}
			}

			if (strcmp("culling", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					int val = std::stoi(piece[1]);
					entityArray[entityCount - 1].culling = val;
				}
			}

			if (strcmp("transparent", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					int val = std::stoi(piece[1]);
					entityArray[entityCount - 1].transparent = val;
					printf("\n transparent %d\n", entityArray[entityCount - 1].transparent);
				}
			}

			if (strcmp("visible", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					int val = std::stoi(piece[1]);
					entityArray[entityCount - 1].visible = val;
				}
			}

			if (strcmp("castShadow", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					int val = std::stoi(piece[1]);
					entityArray[entityCount - 1].castShadow = val;
				}
			}

			if (strcmp("follow", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					int val = std::stoi(piece[1]);
					entityArray[entityCount - 1].follow = val;
				}
			}

			if (strcmp("lookatcam", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					int val = std::stoi(piece[1]);
					entityArray[entityCount - 1].lookatCam = val;
				}
			}

			if (strcmp("reflectable", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					int val = std::stoi(piece[1]);
					entityArray[entityCount - 1].reflectable = val;
				}
			}

			if (strcmp("depth", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					int val = std::stoi(piece[1]);
					entityArray[entityCount - 1].depthMap = val;
				}
			}

			if (strcmp("opaque", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					int val = std::stoi(piece[1]);
					entityArray[entityCount - 1].opaqueMap = val;
				}
			}

			// assign reflect map to mesh
			if (strcmp("reflect", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					int val = std::stoi(piece[1]);
					entityArray[entityCount - 1].reflectMap = val;
				}
			}

			// set water state
			if (strcmp("water", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					int val = std::stoi(piece[1]);
					entityArray[entityCount - 1].isWater = val;
				}
			}

			if (strcmp("lookatpos", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					int cc = sscanf(line, "%s %[^' ,'],%[^' ,'],%s", &piece[0], &piece[1], &piece[2], &piece[3]);
					float val0 = std::stof(piece[1]);
					float val1 = std::stof(piece[2]);
					float val2 = std::stof(piece[3]);
					entityArray[entityCount - 1].lookatPos = glm::vec3(val0, val1, val2);
					entityArray[entityCount - 1].lookatPosState = true;
				}
			}

			if (strcmp("pos", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					int cc = sscanf(line, "%s %[^' ,'],%[^' ,'],%s", &piece[0], &piece[1], &piece[2], &piece[3]);
					float val0 = std::stof(piece[1]);
					float val1 = std::stof(piece[2]);
					float val2 = std::stof(piece[3]);
					entityArray[entityCount - 1].pos = glm::vec3(val0, val1, val2);
				}
			}

			if (strcmp("rot", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					int cc = sscanf(line, "%s %[^' ,'],%[^' ,'],%s", &piece[0], &piece[1], &piece[2], &piece[3]);
					float val0 = std::stof(piece[1]);
					float val1 = std::stof(piece[2]);
					float val2 = std::stof(piece[3]);
					entityArray[entityCount - 1].rot = glm::vec3(val0, val1, val2);
				}
			}

			if (strcmp("scale", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					int cc = sscanf(line, "%s %[^' ,'],%[^' ,'],%s", &piece[0], &piece[1], &piece[2], &piece[3]);
					float val0 = std::stof(piece[1]);
					float val1 = std::stof(piece[2]);
					float val2 = std::stof(piece[3]);
					entityArray[entityCount - 1].scale = glm::vec3(val0, val1, val2);
				}
			}

			if (strcmp("shader", piece[0]) == 0)
			{
				if (entityCount > 0)
				{

					int cc = sscanf(line, "%s %[^' ,'],%s", &piece[0], &piece[1], &piece[2]);


					entityArray[entityCount - 1].shaderAssigned = true;
					strcpy(entityArray[entityCount - 1].vertShader, piece[1]);
					strcpy(entityArray[entityCount - 1].fragShader, piece[2]);
					//continue;
				}
			}

			if (strcmp("uniform", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					int cc = sscanf(line, "%s %[^' ,'],%[^' ,'],%[^' ,'],%[^' ,'],%s", &piece[0], &piece[1], &piece[2], &piece[3], &piece[4], &piece[5]);

					int index = entityArray[entityCount - 1].uniformCount;
					strcpy(entityArray[entityCount - 1].uniformName[index], piece[1]);
					entityArray[entityCount - 1].uniformVal[0][index] = std::stof(piece[2]);
					entityArray[entityCount - 1].uniformVal[1][index] = std::stof(piece[3]);
					entityArray[entityCount - 1].uniformVal[2][index] = std::stof(piece[4]);
					entityArray[entityCount - 1].uniformVal[3][index] = std::stof(piece[5]);

					entityArray[entityCount - 1].uniformCount++;
				}
			}

			if (strcmp("tex0", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					
					entityArray[entityCount - 1].texAssigned[0] = true;
					strcpy(entityArray[entityCount - 1].texName[0], piece[1]);

					if (pieceCount > 2)
						entityArray[entityCount - 1].texWrap[0] = std::stoi(piece[2]);
				}
			}

			if (strcmp("tex1", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					entityArray[entityCount - 1].texAssigned[1] = true;
					strcpy(entityArray[entityCount - 1].texName[1], piece[1]);
					
					if (pieceCount > 2)
						entityArray[entityCount - 1].texWrap[1] = std::stoi(piece[2]);
				}
			}

			if (strcmp("tex2", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					entityArray[entityCount - 1].texAssigned[2] = true;
					strcpy(entityArray[entityCount - 1].texName[2], piece[1]);

					if (pieceCount > 2)
						entityArray[entityCount - 1].texWrap[2] = std::stoi(piece[2]);
				}
			}

			if (strcmp("tex3", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					entityArray[entityCount - 1].texAssigned[3] = true;
					strcpy(entityArray[entityCount - 1].texName[3], piece[1]);

					if (pieceCount > 2)
						entityArray[entityCount - 1].texWrap[3] = std::stoi(piece[2]);
				}
			}

			if (strcmp("tex4", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					entityArray[entityCount - 1].texAssigned[4] = true;
					strcpy(entityArray[entityCount - 1].texName[4], piece[1]);

					if (pieceCount > 2)
						entityArray[entityCount - 1].texWrap[4] = std::stoi(piece[2]);
				}
			}

			if (strcmp("tex5", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					entityArray[entityCount - 1].texAssigned[5] = true;
					strcpy(entityArray[entityCount - 1].texName[5], piece[1]);

					if (pieceCount > 2)
						entityArray[entityCount - 1].texWrap[5] = std::stoi(piece[2]);
				}
			}

			if (strcmp("tex6", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					entityArray[entityCount - 1].texAssigned[6] = true;
					strcpy(entityArray[entityCount - 1].texName[6], piece[1]);

					if (pieceCount > 2)
						entityArray[entityCount - 1].texWrap[6] = std::stoi(piece[2]);
				}
			}

			if (strcmp("tex7", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					entityArray[entityCount - 1].texAssigned[7] = true;
					strcpy(entityArray[entityCount - 1].texName[7], piece[1]);

					if (pieceCount > 2)
						entityArray[entityCount - 1].texWrap[7] = std::stoi(piece[2]);
				}
			}

			if (strcmp("tex8", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					entityArray[entityCount - 1].texAssigned[8] = true;
					strcpy(entityArray[entityCount - 1].texName[8], piece[1]);

					if (pieceCount > 2)
						entityArray[entityCount - 1].texWrap[8] = std::stoi(piece[2]);
				}
			}

			if (strcmp("tex9", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					entityArray[entityCount - 1].texAssigned[9] = true;
					strcpy(entityArray[entityCount - 1].texName[9], piece[1]);

					if (pieceCount > 2)
						entityArray[entityCount - 1].texWrap[9] = std::stoi(piece[2]);
				}
			}

			if (strcmp("tex10", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					entityArray[entityCount - 1].texAssigned[10] = true;
					strcpy(entityArray[entityCount - 1].texName[10], piece[1]);

					if (pieceCount > 2)
						entityArray[entityCount - 1].texWrap[10] = std::stoi(piece[2]);
				}
			}

			if (strcmp("tex11", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					entityArray[entityCount - 1].texAssigned[11] = true;
					strcpy(entityArray[entityCount - 1].texName[11], piece[1]);

					if (pieceCount > 2)
						entityArray[entityCount - 1].texWrap[11] = std::stoi(piece[2]);
				}
			}

			if (strcmp("tex12", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					entityArray[entityCount - 1].texAssigned[12] = true;
					strcpy(entityArray[entityCount - 1].texName[12], piece[1]);

					if (pieceCount > 2)
						entityArray[entityCount - 1].texWrap[12] = std::stoi(piece[2]);
				}
			}

			if (strcmp("tex13", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					entityArray[entityCount - 1].texAssigned[13] = true;
					strcpy(entityArray[entityCount - 1].texName[13], piece[1]);

					if (pieceCount > 2)
						entityArray[entityCount - 1].texWrap[13] = std::stoi(piece[2]);
				}
			}

			if (strcmp("tex14", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					entityArray[entityCount - 1].texAssigned[14] = true;
					strcpy(entityArray[entityCount - 1].texName[14], piece[1]);

					if (pieceCount > 2)
						entityArray[entityCount - 1].texWrap[14] = std::stoi(piece[2]);
				}
			}

			if (strcmp("tex15", piece[0]) == 0)
			{
				if (entityCount > 0)
				{
					entityArray[entityCount - 1].texAssigned[15] = true;
					strcpy(entityArray[entityCount - 1].texName[15], piece[1]);

					if (pieceCount > 2)
						entityArray[entityCount - 1].texWrap[15] = std::stoi(piece[2]);
				}
			}
			if (entityArray[entityCount - 1].texAssigned[1]) printf("%s \n", entityArray[entityCount - 1].texName[1]);
		}
	}

	fclose(file);
}

bool EntityEntry::operator ==(EntityEntry &other)
{
	if (strcmp(meshName, other.meshName) != 0) return false;
	if (collision != other.collision) return false;
	if (culling != other.culling) return false;
	if (transparent != other.transparent) return false;
	if (visible != other.visible) return false;
	if (castShadow != other.castShadow) return false;

	if (pos != other.pos) return false;
	if (scale != other.scale) return false; 
	if (rot != other.rot) return false;

	if (strcmp(vertShader, other.vertShader) != 0) return false;
	if (strcmp(fragShader, other.fragShader) != 0) return false;
	if (uniformCount != other.uniformCount) return false;
	for (int ind = 0; ind < uniformCount; ind++)
	{
		if (strcmp(uniformName[ind], other.uniformName[ind]) != 0) return false;
		for (int i = 0; i < 4; i++)
		{
			if (uniformVal[i][ind] != other.uniformVal[i][ind]) return false;
		}
	}

	for (int n = 0; n < 16; n++)
	{
		if (strcmp(texName[n], other.texName[n]) != 0) return false;
		if (texWrap[n] != other.texWrap[n]) return false;
	}

	return true;
}