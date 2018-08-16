#include "Mesh.h"
#include <string>
#include "VBOIndexer.h"
#include <ctime>

#include "gtc/matrix_transform.hpp"
#include "gtx/transform.hpp"

using namespace ENGINE;

bool loadOBJ(
	const char * path,
	MeshFileData &store
	)
{
	Stack< unsigned int > vertexIndices, uvIndices, normalIndices, colorIndices, tangentIndices;
	Stack< glm::vec3 > temp_vertices;
	Stack< glm::vec2 > temp_uvs;
	Stack< glm::vec3 > temp_normals;
	Stack< glm::vec3 > temp_colors;
	Stack< glm::vec3 > temp_tangents;

	FILE * file = NULL;
	fopen_s(&file, path, "r");
	if (file == NULL)
	{
		printf("Impossible to open the file !\n");
		return false;
	}
	char lineHeader[1024];
	int res;
	bool attribute[10] = { false  };
	unsigned int vertexIndex[20], uvIndex[20], normalIndex[20], colorIndex[20], tangentIndex[20], binormalIndex[20];

	while (1)
	{

		// read the first word of the line
		
		res = fscanf_s(file, "%s", lineHeader, 1024);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		
		if (strcmp(lineHeader, "v") == 0) // vertex 3-space position
		{
			glm::vec3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.add(vertex*glm::vec3(-1,1,-1));
			attribute[0] = true;
		}
		else if (strcmp(lineHeader, "vt") == 0) // vertex uv coord
		{
			glm::vec2 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.add(uv);
			attribute[1] = true;
		}
		else if (strcmp(lineHeader, "vn") == 0) // vertex normal
		{
			glm::vec3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.add(normal*glm::vec3(-1, 1, -1));
			attribute[2] = true;
		}
		else if (strcmp(lineHeader, "vc") == 0) // vertex color
		{
			glm::vec3 color;
			fscanf_s(file, "%f %f %f\n", &color.x, &color.y, &color.z);
			temp_colors.add(color);
			attribute[3] = true;
		}
		else if (strcmp(lineHeader, "v_tan") == 0) // vertex color
		{
			glm::vec3 tangent;
			fscanf_s(file, "%f %f %f\n", &tangent.x, &tangent.y, &tangent.z);
			temp_colors.add(tangent);
			attribute[4] = true;
		}
		else if (strcmp(lineHeader, "v_bin") == 0) // vertex color
		{
			glm::vec3 binormal;
			fscanf_s(file, "%f %f %f\n", &binormal.x, &binormal.y, &binormal.z);
			temp_colors.add(binormal);
			attribute[5] = true;
		}
		else if (strcmp(lineHeader, "f") == 0)
		{

			char matches;

			// no color, no tangents
			if (!attribute[3] &&
				!attribute[4])
			{
				int a = 0;
				matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
					&vertexIndex[0], &uvIndex[0], &normalIndex[0],
					&vertexIndex[1], &uvIndex[1], &normalIndex[1],
					&vertexIndex[2], &uvIndex[2], &normalIndex[2],
					&vertexIndex[3], &uvIndex[3], &normalIndex[3],
					&vertexIndex[4], &uvIndex[4], &normalIndex[4],
					&vertexIndex[5], &uvIndex[5], &normalIndex[5],
					&vertexIndex[6], &uvIndex[6], &normalIndex[6],
					&vertexIndex[7], &uvIndex[7], &normalIndex[7],
					&vertexIndex[8], &uvIndex[8], &normalIndex[8]);
				a = 9;
				matches += fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n",
					&vertexIndex[a + 0], &uvIndex[a + 0], &normalIndex[a + 0],
					&vertexIndex[a + 1], &uvIndex[a + 1], &normalIndex[a + 1],
					&vertexIndex[a + 2], &uvIndex[a + 2], &normalIndex[a + 2],
					&vertexIndex[a + 3], &uvIndex[a + 3], &normalIndex[a + 3],
					&vertexIndex[a + 4], &uvIndex[a + 4], &normalIndex[a + 4],
					&vertexIndex[a + 5], &uvIndex[a + 5], &normalIndex[a + 5],
					&vertexIndex[a + 6], &uvIndex[a + 6], &normalIndex[a + 6],
					&vertexIndex[a + 7], &uvIndex[a + 7], &normalIndex[a + 7],
					&vertexIndex[a + 8], &uvIndex[a + 8], &normalIndex[a + 8]);
			}
			// yes color, no tangents
			if (attribute[3] &&
				!attribute[4])
			{
				
				int a = 0;
				matches = fscanf_s(file, "%d/%d/%d/%d %d/%d/%d/%d %d/%d/%d/%d %d/%d/%d/%d %d/%d/%d/%d %d/%d/%d/%d %d/%d/%d/%d %d/%d/%d/%d %d/%d/%d/%d\n",
					&vertexIndex[0], &uvIndex[0], &normalIndex[0], &colorIndex[0],
					&vertexIndex[1], &uvIndex[1], &normalIndex[1], &colorIndex[1],
					&vertexIndex[2], &uvIndex[2], &normalIndex[2], &colorIndex[2],
					&vertexIndex[3], &uvIndex[3], &normalIndex[3], &colorIndex[3],
					&vertexIndex[4], &uvIndex[4], &normalIndex[4], &colorIndex[4],
					&vertexIndex[5], &uvIndex[5], &normalIndex[5], &colorIndex[5],
					&vertexIndex[6], &uvIndex[6], &normalIndex[6], &colorIndex[6],
					&vertexIndex[7], &uvIndex[7], &normalIndex[7], &colorIndex[7],
					&vertexIndex[8], &uvIndex[8], &normalIndex[8], &colorIndex[8]);
			}
			// no color, yes tangents
			if (!attribute[3] &&
				attribute[4])
			{
				
				int a = 0;
				matches = fscanf_s(file, "%d/%d/%d/%d %d/%d/%d/%d %d/%d/%d/%d %d/%d/%d/%d %d/%d/%d/%d %d/%d/%d/%d %d/%d/%d/%d %d/%d/%d/%d %d/%d/%d/%d\n",
					&vertexIndex[0], &uvIndex[0], &normalIndex[0], &tangentIndex[0],
					&vertexIndex[1], &uvIndex[1], &normalIndex[1], &tangentIndex[1],
					&vertexIndex[2], &uvIndex[2], &normalIndex[2], &tangentIndex[2],
					&vertexIndex[3], &uvIndex[3], &normalIndex[3], &tangentIndex[3],
					&vertexIndex[4], &uvIndex[4], &normalIndex[4], &tangentIndex[4],
					&vertexIndex[5], &uvIndex[5], &normalIndex[5], &tangentIndex[5],
					&vertexIndex[6], &uvIndex[6], &normalIndex[6], &tangentIndex[6],
					&vertexIndex[7], &uvIndex[7], &normalIndex[7], &tangentIndex[7],
					&vertexIndex[8], &uvIndex[8], &normalIndex[8], &tangentIndex[8]);
			}
			if (matches % 3 != 0)
			{
				printf("File can't be read  : ( Try exporting with other options\n");
				return false;
			}

			// no color, no tangents
			if (!attribute[3] &&
				!attribute[4])
			{
				if (matches >= 9)
				{
					for (int n = 0; n < (matches - 6) / 3; n++)
					{
						vertexIndices.add(vertexIndex[0]);

						vertexIndices.add(vertexIndex[n + 1]);
						vertexIndices.add(vertexIndex[n + 2]);

						uvIndices.add(uvIndex[0]);

						uvIndices.add(uvIndex[n + 1]);
						uvIndices.add(uvIndex[n + 2]);

						normalIndices.add(normalIndex[0]);

						normalIndices.add(normalIndex[n + 1]);
						normalIndices.add(normalIndex[n + 2]);

					}
				}
			}

			// yes color, no tangents
			if (attribute[3] &&
				!attribute[4])
			{
				if (matches >= 12)
				{
					for (int n = 0; n < (matches - 8) / 4; n++)
					{
						vertexIndices.add(vertexIndex[0]);
						vertexIndices.add(vertexIndex[n + 1]);
						vertexIndices.add(vertexIndex[n + 2]);

						uvIndices.add(uvIndex[0]);
						uvIndices.add(uvIndex[n + 1]);
						uvIndices.add(uvIndex[n + 2]);

						normalIndices.add(normalIndex[0]);
						normalIndices.add(normalIndex[n + 1]);
						normalIndices.add(normalIndex[n + 2]);

						colorIndices.add(colorIndex[0]);
						colorIndices.add(colorIndex[n + 1]);
						colorIndices.add(colorIndex[n + 2]);

					}
				}
			}

			// yes color, no tangents
			if (!attribute[3] &&
				attribute[4])
			{
				if (matches >= 12)
				{
					for (int n = 0; n < (matches - 8) / 4; n++)
					{
						vertexIndices.add(vertexIndex[0]);
						vertexIndices.add(vertexIndex[n + 1]);
						vertexIndices.add(vertexIndex[n + 2]);

						uvIndices.add(uvIndex[0]);
						uvIndices.add(uvIndex[n + 1]);
						uvIndices.add(uvIndex[n + 2]);

						normalIndices.add(normalIndex[0]);
						normalIndices.add(normalIndex[n + 1]);
						normalIndices.add(normalIndex[n + 2]);

						tangentIndices.add(tangentIndex[0]);
						tangentIndices.add(tangentIndex[n + 1]);
						tangentIndices.add(tangentIndex[n + 2]);

					}
				}
			}
		}
	}
	fclose(file);
	
	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size; i++)
	{
		unsigned int vertexIndex = vertexIndices.item[i];
		glm::vec3 vertex = temp_vertices.item[vertexIndex - 1];
		store.vertices.add(vertex);
	}

	// For each uv of each triangle
	for (unsigned int i = 0; i < uvIndices.size; i++)
	{
		unsigned int uvIndex = uvIndices.item[i];
		glm::vec2 uv = temp_uvs.item[uvIndex - 1];
		store.uvs.add(uv);
	}

	// For each normal of each triangle
	for (unsigned int i = 0; i < normalIndices.size; i++)
	{
		unsigned int normalIndex = normalIndices.item[i];
		glm::vec3 normal = temp_normals.item[normalIndex - 1];
		store.normals.add(normal);
	}

	// For each color of each triangle
	for (unsigned int i = 0; i < colorIndices.size; i++)
	{
		unsigned int colorIndex = colorIndices.item[i];
		glm::vec3 color = temp_colors.item[colorIndex - 1];
		store.colors.add(color);
	}

}

// export selected mesh to obj file
void MeshManager::exportMesh(int index)
{
	char* path;
	path = new char[256];
	strcpy(path, meshArray[index].name);
	strcat(path, ".obj");
	FILE * file = NULL;
	fopen_s(&file, path, "w+");

	Stack<unsigned short> indices = meshArray[index].indices;
	Stack<glm::vec3> ind_vertex = meshArray[index].ind_vertex;
	Stack<glm::vec2> ind_uv = meshArray[index].ind_uv;
	Stack<glm::vec3> ind_normal = meshArray[index].ind_normal;
	Stack<glm::vec3> ind_color = meshArray[index].ind_GI;

	for (int n = 0; n < ind_vertex.size; n++)
	{
		glm::vec4 update = meshArray[index].modelMatrix * glm::vec4(ind_vertex.item[n], 1.0);
		ind_vertex.item[n] = glm::vec3(update.x, update.y, update.z);
	}

	for (int n = 0; n < ind_normal.size; n++)
	{
		glm::vec4 update = meshArray[index].transformMatrix * glm::vec4(ind_normal.item[n], 1.0);
		ind_normal.item[n] = glm::vec3(update.x, update.y, update.z);
	}

	fputs("# Exported from Katina editor\n", file);

	// add date to file
	time_t t = time(0);   // get time now
	struct tm * now = localtime(&t);
	int a = now->tm_year + 1900;
	int b = now->tm_mon + 1;
	int c = now->tm_mday;

	std::string sday = std::to_string(c);
	std::string syear = std::to_string(a);

	char date[60];

	char *smonth;
	if (b == 1) smonth = "January";
	if (b == 2) smonth = "February";
	if (b == 3) smonth = "March";
	if (b == 4) smonth = "April";
	if (b == 5) smonth = "May";
	if (b == 6) smonth = "June";
	if (b == 7) smonth = "July";
	if (b == 8) smonth = "August";
	if (b == 9) smonth = "September";
	if (b == 10) smonth = "October";
	if (b == 11) smonth = "November";
	if (b == 12) smonth = "December";

	strcpy(date, "# Created on ");
	strcat(date, smonth);
	strcat(date, " ");
	strcat(date, sday.c_str());
	strcat(date, ", ");
	strcat(date, syear.c_str());
	strcat(date, "\n");

	fputs(date, file);

	// add mesh name
	fputs("\n",file);
	fputs("#\n", file);

	char meshName[60];
	strcpy(meshName, "# object ");
	strcat(meshName, meshArray[index].name);
	strcat(meshName, "\n");
	fputs(meshName, file);

	fputs("#\n", file);
	fputs("\n", file);

	

	
	
	char vert[256];
	std::string v;

	// write min and max bounds of mesh
	strcpy(vert, "min ");
	v = std::to_string(meshArray[index].minpos.x);
	strcat(vert, v.c_str());
	strcat(vert, " ");
	v = std::to_string(meshArray[index].minpos.y);
	strcat(vert, v.c_str());
	strcat(vert, " ");
	v = std::to_string(meshArray[index].minpos.z);
	strcat(vert, v.c_str());
	strcat(vert, "\n");

	fputs(vert, file);

	strcpy(vert, "max ");
	v = std::to_string(meshArray[index].maxpos.x);
	strcat(vert, v.c_str());
	strcat(vert, " ");
	v = std::to_string(meshArray[index].maxpos.y);
	strcat(vert, v.c_str());
	strcat(vert, " ");
	v = std::to_string(meshArray[index].maxpos.z);
	strcat(vert, v.c_str());
	strcat(vert, "\n");

	fputs(vert, file);
	fputs("\n\n", file);

	// add vertices
	for (int n = 0; n < ind_vertex.size; n++)
	{
		strcpy(vert, "v  ");
		v = std::to_string(-ind_vertex.item[n].x);
		strcat(vert, v.c_str());
		strcat(vert, " ");
		v = std::to_string(ind_vertex.item[n].y);
		strcat(vert, v.c_str());
		strcat(vert, " ");
		v = std::to_string(-ind_vertex.item[n].z);
		strcat(vert, v.c_str());
		strcat(vert, "\n");
		
		fputs(vert, file);
	}

	char vertcount[30];
	strcpy(vertcount, "# ");
	strcat(vertcount, std::to_string(ind_vertex.size).c_str());
	strcat(vertcount, " vertices\n\n");
	fputs(vertcount, file);

	// add normals
	for (int n = 0; n < ind_normal.size; n++)
	{
		strcpy(vert, "vn ");
		v = std::to_string(-ind_normal.item[n].x);
		strcat(vert, v.c_str());
		strcat(vert, " ");
		v = std::to_string(ind_normal.item[n].y);
		strcat(vert, v.c_str());
		strcat(vert, " ");
		v = std::to_string(-ind_normal.item[n].z);
		strcat(vert, v.c_str());
		strcat(vert, "\n");

		fputs(vert, file);
	}

	char normcount[30];
	strcpy(normcount, "# ");
	strcat(normcount, std::to_string(ind_normal.size).c_str());
	strcat(normcount, " normals\n\n");
	fputs(normcount, file);


	// add uvs
	for (int n = 0; n < ind_uv.size; n++)
	{
		strcpy(vert, "vt ");
		v = std::to_string(ind_uv.item[n].x);
		strcat(vert, v.c_str());
		strcat(vert, " ");
		v = std::to_string(ind_uv.item[n].y);
		strcat(vert, v.c_str());
		strcat(vert, " ");
		v = std::to_string(0.0f);
		strcat(vert, v.c_str());
		strcat(vert, "\n");

		fputs(vert, file);
	}

	char uvcount[30];
	strcpy(uvcount, "# ");
	strcat(uvcount, std::to_string(ind_uv.size).c_str());
	strcat(uvcount, " texture coords\n\n");
	fputs(uvcount, file);

	// add colors
	for (int n = 0; n < ind_color.size; n++)
	{
		strcpy(vert, "vc ");
		v = std::to_string(ind_color.item[n].x);
		strcat(vert, v.c_str());
		strcat(vert, " ");
		v = std::to_string(ind_color.item[n].y);
		strcat(vert, v.c_str());
		strcat(vert, " ");
		v = std::to_string(ind_color.item[n].z);
		strcat(vert, v.c_str());
		strcat(vert, "\n");

		fputs(vert, file);
	}

	char colorcount[30];
	strcpy(colorcount, "# ");
	strcat(colorcount, std::to_string(ind_color.size).c_str());
	strcat(colorcount, " colors\n\n");
	fputs(colorcount, file);

	// add indices
	strcpy(meshName, "g ");
	strcat(meshName, meshArray[index].name);
	strcat(meshName, "\n");
	fputs(meshName, file);

	fputs("s 1\n", file);	
	// no color, no tangent
	if (ind_color.size == 0)
	{
		for (int n = 0; n < indices.size; n += 3)
		{
			strcpy(vert, "f ");
			v = std::to_string(indices.item[n] + 1);
			strcat(vert, v.c_str());
			strcat(vert, "/");
			strcat(vert, v.c_str());
			strcat(vert, "/");
			strcat(vert, v.c_str());
			strcat(vert, " ");
			v = std::to_string(indices.item[n + 1] + 1);
			strcat(vert, v.c_str());
			strcat(vert, "/");
			strcat(vert, v.c_str());
			strcat(vert, "/");
			strcat(vert, v.c_str());
			strcat(vert, " ");
			v = std::to_string(indices.item[n + 2] + 1);
			strcat(vert, v.c_str());
			strcat(vert, "/");
			strcat(vert, v.c_str());
			strcat(vert, "/");
			strcat(vert, v.c_str());
			strcat(vert, "\n");

			fputs(vert, file);
		}
	}
	if (ind_color.size > 0)
	{
		for (int n = 0; n < indices.size; n += 3)
		{
			strcpy(vert, "f ");
			v = std::to_string(indices.item[n] + 1);
			strcat(vert, v.c_str());
			strcat(vert, "/");
			strcat(vert, v.c_str());
			strcat(vert, "/");
			strcat(vert, v.c_str());
			strcat(vert, "/");
			strcat(vert, v.c_str());
			strcat(vert, " ");
			v = std::to_string(indices.item[n + 1] + 1);
			strcat(vert, v.c_str());
			strcat(vert, "/");
			strcat(vert, v.c_str());
			strcat(vert, "/");
			strcat(vert, v.c_str());
			strcat(vert, "/");
			strcat(vert, v.c_str());
			strcat(vert, " ");
			v = std::to_string(indices.item[n + 2] + 1);
			strcat(vert, v.c_str());
			strcat(vert, "/");
			strcat(vert, v.c_str());
			strcat(vert, "/");
			strcat(vert, v.c_str());
			strcat(vert, "/");
			strcat(vert, v.c_str());
			strcat(vert, "\n");

			fputs(vert, file);
		}
	}
	char indexcount[30];
	strcpy(indexcount, "# ");
	strcat(indexcount, std::to_string(indices.size).c_str());
	strcat(indexcount, " polygons\n\n");
	fputs(indexcount, file);


	fclose(file);
}

MeshObj::MeshObj()
{
	id = -1;
	//indices = Stack<unsigned short>();
	//ind_vertex = Stack<glm::vec3>();

	transparent = false;
	culling = 1;
	depthWrite = true;
	depthRead = GL_LESS;
	visible = true;
	collision = true;
	castShadow = true;
	strcpy(name, "not set");
	lookatCam = false;
	lookatPosState = false;
	follow = false;
	reflectable = true;
	isWater = false;

	type = STANDARD;
	frustumCulled = false;

	useTangents = false;
	useVertexLight = false;

	for (int n = 0; n < 24; n++)
		texture[n] = -2;

	for (int n = 0; n < 32; n++)
		constantArray[n] = ShaderConst();
	constantArraySize = 32;
	constantCount = 0;

	for (int n = 0; n < 32; n++)
		constantMatArray[n] = ShaderMat();
	constantMatCount = 0;

	pivot = glm::vec3(0);
	pos = glm::vec3(0);
	scale = glm::vec3(1);
	rot = glm::vec3(0);

	//shader = 0;
	shaderIndex = 0;
	isInstance = false;
	instanceOf = NULL;

}

MeshObj::MeshObj(const MeshObj &other)
{
	id = other.id;
	//name = other.name;
	strcpy(name, other.name);

	indices = Stack<unsigned short>(other.indices ,true);
	ind_vertex = Stack<glm::vec3>(other.ind_vertex, true);
	ind_uv = Stack<glm::vec2>(other.ind_uv, true);
	ind_normal = Stack<glm::vec3>(other.ind_normal, true);
	ind_GI = Stack<glm::vec3>(other.ind_GI, true);

	vertexBuffer = other.vertexBuffer;
	uvBuffer = other.uvBuffer;
	normalBuffer = other.normalBuffer;
	elementBuffer = other.elementBuffer;
	vertGIBuffer = other.vertGIBuffer;
	tangentBuffer = other.tangentBuffer;
	binormalBuffer = other.binormalBuffer;

	pos = other.pos;
	rot = other.rot;
	scale = other.scale;
	pivot = other.pivot;

	transparent = other.transparent;
	visible = other.visible;
	culling = other.culling;
	collision = other.collision;
	castShadow = other.castShadow;
	depthWrite = other.depthWrite;
	depthRead = other.depthRead;
	lookatCam = other.lookatCam;
	lookatPosState = other.lookatPosState;
	lookatPos = other.lookatPos;
	follow = other.follow;
	followPos = other.followPos;
	reflectable = other.reflectable;
	isWater = other.isWater;
	type = other.type;
	frustumCulled = other.frustumCulled;

	useTangents = other.useTangents;
	useVertexLight = other.useVertexLight;

	minpos = other.minpos;
	maxpos = other.maxpos;

	modelMatrix = other.modelMatrix;
	transformMatrix = other.transformMatrix;

	// textures
	for (int n = 0; n < 24; n++)
		texture[n] = other.texture[n];
	//shader = other.shader;
	shaderIndex = other.shaderIndex;

	// non-matrix uniforms
	for (int n = 0; n < 32; n++)
		constantArray[n] = other.constantArray[n];
	constantArraySize = other.constantArraySize;
	constantCount = other.constantCount;

	// matrix uniforms
	for (int n = 0; n < 32; n++)
		constantMatArray[n] = other.constantMatArray[n];
	constantMatCount = other.constantMatCount;

	isInstance = other.isInstance;
	instanceOf = other.instanceOf;

}

MeshObj MeshObj::instanceCopy()
{
	MeshObj temp;
	//temp.name = name;

	temp.pos = pos;
	temp.rot = rot;
	temp.scale = scale;
	temp.pivot = pivot;

	temp.visible = visible;

	

	for (int n = 0; n < 16; n++)
		temp.texture[n] = texture[n];
	temp.shaderIndex = shaderIndex;

	for (int n = 0; n < 32; n++)
		temp.constantArray[n] = constantArray[n];

	temp.constantArraySize = constantArraySize;
	temp.constantCount = constantCount;

	temp.isInstance = true;
	temp.instanceOf = NULL;

	return temp;
}

void MeshObj::setShaderConst(Shader *shader, char *name, float v0, float v1, float v2, float v3)
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

void MeshObj::setShaderMat(Shader *shader, char *name, glm::mat4 val)
{
	GLint loc;
	loc = shader->getUniformLocation(name);


	// check if uniform name already exists on this object
	for (int n = 0; n < constantMatCount; n++)
	{
		//if (constantMatArray[n].name != NULL)
		{
			if (strcmp(name, constantMatArray[n].name) == 0)
			{
				constantMatArray[n].value = val;
				constantMatArray[n].location = loc;

				return;
			}
		}
	}

	// if uniform name does not already exist... add it
	constantMatArray[constantMatCount].name = name;
	constantMatArray[constantMatCount].value = val;
	constantMatArray[constantMatCount].location = loc;

	constantMatCount++;

	return;
}

void MeshObj::setVisible(bool state)
{
	visible = state;
}

bool MeshObj::getVisible()
{
	return visible;
}

void MeshObj::setCullMode(char state)
{
	culling = state;
}

char MeshObj::getCullMode()
{
	return culling;
}

void MeshObj::setDepthWrite(bool state)
{
	depthWrite = state;
}

bool MeshObj::getDepthWrite()
{
	return depthWrite;
}

void MeshObj::setTransparency(bool state)
{
	transparent = state;
}

bool MeshObj::getTransparency()
{
	return transparent;
}

void MeshObj::setShader(int shaderInd)
{
	//shader = &shaderobj;
	shaderIndex = shaderInd;
}

int MeshObj::getShader()
{
	return shaderIndex;
}

void MeshObj::setDepthRead(char mode)
{
	depthRead = mode;
}

char MeshObj::getDepthRead()
{
	return depthRead;
}

// calculate minimum and maximum bounds of mesh vertex positions
void MeshObj::calcMinMax()
{
	// find minimum values in mesh vertices
	minpos = glm::vec3(100000.0f);
	for (int n = 0; n < indices.size; n++)
	{
		int index = indices.item[n];
		if (ind_vertex.item[index].x < minpos.x) minpos.x = ind_vertex.item[index].x;
		if (ind_vertex.item[index].y < minpos.y) minpos.y = ind_vertex.item[index].y;
		if (ind_vertex.item[index].z < minpos.z) minpos.z = ind_vertex.item[index].z;
	}

	// find maximum values in mesh vertices
	maxpos = glm::vec3(-100000.0f);
	for (int n = 0; n < indices.size; n++)
	{
		int index = indices.item[n];
		if (ind_vertex.item[index].x > maxpos.x) maxpos.x = ind_vertex.item[index].x;
		if (ind_vertex.item[index].y > maxpos.y) maxpos.y = ind_vertex.item[index].y;
		if (ind_vertex.item[index].z > maxpos.z) maxpos.z = ind_vertex.item[index].z;
	}
}

float MeshObj::getMinX()
{
	return minpos.x;
}

float MeshObj::getMaxX()
{
	return maxpos.x;
}

float MeshObj::getMinY()
{
	return minpos.y;
}

float MeshObj::getMaxY()
{
	return maxpos.y;
}

float MeshObj::getMinZ()
{
	return minpos.z;
}

float MeshObj::getMaxZ()
{
	return maxpos.z;
}

void MeshObj::setPosition(float x, float y, float z)
{
	pos = glm::vec3(x, y, z);
}

void MeshObj::setPosition(glm::vec3 p)
{
	pos = p;
}

glm::vec3 MeshObj::getPosition()
{
	return pos;
}

void MeshObj::setRotation(float x, float y, float z)
{
	rot = glm::vec3(x, y, z);
}

void MeshObj::setRotation(glm::vec3 p)
{
	rot = p;
}

glm::vec3 MeshObj::getRotation()
{
	return rot;
}

void MeshObj::setScale(float x, float y, float z)
{
	scale = glm::vec3(x, y, z);
}

void MeshObj::setScale(glm::vec3 p)
{
	scale = p;
}

glm::vec3 MeshObj::getScale()
{
	return scale;
}

void MeshObj::setPivot(float x, float y, float z)
{
	pivot = glm::vec3(x, y, z);
}

void MeshObj::setPivot(glm::vec3 p)
{
	pivot = p;
}

glm::vec3 MeshObj::getPivot()
{
	return pivot;
}

glm::mat4 MeshObj::updateModelMatrix()
{
	glm::mat4 scaling;
	glm::mat4 translating;
	glm::mat4 xrotation;
	glm::mat4 yrotation;
	glm::mat4 zrotation;

	glm::mat4 pivoting;
	glm::mat4 unpivoting;

	glm::mat4 pivotingNorm;
	glm::mat4 unpivotingNorm;

	glm::vec3 rad_rot = rot*3.14159f / 180.0f;
	
	scaling = glm::mat4(
		scale.x, 0.0, 0.0, 0.0,
		0.0, scale.y, 0.0, 0.0,
		0.0, 0.0, scale.z, 0.0,
		0.0, 0.0, 0.0, 1.0);

	translating = glm::mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		pos.x, pos.y, pos.z, 1.0);

	xrotation = glm::mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, cos(rad_rot.x), sin(rad_rot.x), 0.0,
		0.0, -sin(rad_rot.x), cos(rad_rot.x), 0.0,
		0.0, 0.0, 0.0, 1.0);

	yrotation = glm::mat4(
		cos(rad_rot.y), 0.0, -sin(rad_rot.y), 0.0,
		0.0, 1.0, 0.0, 0.0,
		sin(rad_rot.y), 0.0, cos(rad_rot.y), 0.0,
		0.0, 0.0, 0.0, 1.0);

	zrotation = glm::mat4(
		cos(rad_rot.z), sin(rad_rot.z), 0.0, 0.0,
		-sin(rad_rot.z), cos(rad_rot.z), 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0);

	pivoting = glm::mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		-pivot.x-pos.x, -pivot.y-pos.y, -pivot.z-pos.z, 1.0);

	unpivoting = glm::mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		pivot.x+pos.x, pivot.y+pos.y, pivot.z+pos.z, 1.0);

	pivotingNorm = glm::mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		-pivot.x , -pivot.y , -pivot.z , 1.0);

	unpivotingNorm = glm::mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		pivot.x , pivot.y , pivot.z , 1.0);

	transformMatrix =   zrotation * yrotation * xrotation;

	modelMatrix =  translating *transformMatrix* scaling;

	//modelMatrix = translating * scaling * xrotation * yrotation * zrotation;
	return modelMatrix;
}

glm::mat4 MeshObj::updateModelMatrix(glm::mat4 transform)
{
	glm::mat4 scaling;
	glm::mat4 translating;

	glm::mat4 pivoting;
	glm::mat4 unpivoting;

	glm::mat4 pivotingNorm;
	glm::mat4 unpivotingNorm;

	glm::vec3 rad_rot = rot*3.14159f / 180.0f;

	scaling = glm::mat4(
		scale.x, 0.0, 0.0, 0.0,
		0.0, scale.y, 0.0, 0.0,
		0.0, 0.0, scale.z, 0.0,
		0.0, 0.0, 0.0, 1.0);

	translating = glm::mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		pos.x, pos.y, pos.z, 1.0);
/*
	pivoting = glm::mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		-pivot.x - pos.x, -pivot.y - pos.y, -pivot.z - pos.z, 1.0);

	unpivoting = glm::mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		pivot.x + pos.x, pivot.y + pos.y, pivot.z + pos.z, 1.0);

	pivotingNorm = glm::mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		-pivot.x, -pivot.y, -pivot.z, 1.0);

	unpivotingNorm = glm::mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		pivot.x, pivot.y, pivot.z, 1.0);
*/
	transformMatrix =  transform ;

	modelMatrix = translating *transform  *  scaling ;

	return modelMatrix;
}

// return transform matrix only; no translation or scaling
glm::mat4 MeshObj::lookat(glm::vec3 lookatpos, float z_rotation)
{
	glm::vec3 lookvec = lookatpos - pos;
	float x_rotation = -3.1415/2.0+atan2(glm::length(glm::vec2(lookvec.x, lookvec.z)), lookvec.y);
	float y_rotation = atan2(lookvec.x, lookvec.z);


	glm::mat4 xrotation, yrotation, zrotation;
	xrotation = glm::mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, cos(x_rotation), sin(x_rotation), 0.0,
		0.0, -sin(x_rotation), cos(x_rotation), 0.0,
		0.0, 0.0, 0.0, 1.0);

	yrotation = glm::mat4(
		cos(y_rotation), 0.0, -sin(y_rotation), 0.0,
		0.0, 1.0, 0.0, 0.0,
		sin(y_rotation), 0.0, cos(y_rotation), 0.0,
		0.0, 0.0, 0.0, 1.0);

	zrotation = glm::mat4(
		cos(z_rotation), sin(z_rotation), 0.0, 0.0,
		-sin(z_rotation), cos(z_rotation), 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0);

	glm::mat4 fin;
	if (abs(z_rotation) > 0.0001) 
		fin = yrotation*xrotation*zrotation; // this works perfectly
	else
		fin = yrotation*xrotation; // if no z_rotation specified, don't bother multiplying

	transformMatrix = fin;
	

	return fin;
}

void MeshObj::updateShaderUniforms(Shader *shader)
{
	for (int n = 0; n < constantCount; n++)
	{

		constantArray[n].location = shader->getUniformLocation(constantArray[n].name);

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
	for (int n = 0; n < constantMatCount; n++)
	{
		constantMatArray[n].location = shader->getUniformLocation(constantMatArray[n].name);
		glUniformMatrix4fv(constantMatArray[n].location, 1, GL_FALSE, &constantMatArray[n].value[0][0]);
	}
}


// calculate mesh tangent and binormals
void MeshObj::getTangents()
{
	for (int index = 0; index < indices.size; index += 3)
	{
		glm::vec3 v0 = ind_vertex.item[indices.item[index]];
		glm::vec3 v1 = ind_vertex.item[indices.item[index + 1]];
		glm::vec3 v2 = ind_vertex.item[indices.item[index + 2]];

		glm::vec2 uv0 = ind_uv.item[indices.item[index]];
		glm::vec2 uv1 = ind_uv.item[indices.item[index + 1]];
		glm::vec2 uv2 = ind_uv.item[indices.item[index + 2]];

		glm::vec3 dpos0 = v1 - v0;
		glm::vec3 dpos1 = v2 - v0;

		glm::vec2 duv0 = uv1 - uv0;
		glm::vec2 duv1 = uv2 - uv0;

		float r = 1.f / (duv0.x*duv1.y - duv0.y*duv1.x);
		glm::vec3 tangent = glm::normalize(r * (dpos0*duv1.y - dpos1 * duv0.y));
		glm::vec3 binormal = glm::normalize(r * (dpos1*duv0.x - dpos0 * duv1.x));

		ind_tangent.item[indices.item[index]] = tangent;
		ind_tangent.item[indices.item[index + 1]] = tangent;
		ind_tangent.item[indices.item[index + 2]] = tangent;

		ind_binormal.item[indices.item[index]] = binormal;
		ind_binormal.item[indices.item[index + 1]] = binormal;
		ind_binormal.item[indices.item[index + 2]] = binormal;
	}

	useTangents = true;
}

// **************** Mesh Manager	

int MeshManager::add(MeshObj mesh)
{
	if (meshCount + 2 > arraySize)
	{
		MeshObj *temp = new MeshObj[arraySize * 2];
		for (int n = 0; n < arraySize; n++)
			temp[n] = meshArray[n];

		delete[] meshArray;
		arraySize *= 2;
		meshArray = temp;
	}
	meshArray[meshCount] = mesh;
	meshCount++;

	return meshCount - 1;
}

int MeshManager::loadMesh(char *name)
{
	//check to see if shader already exists
	for (int n = 0; n < meshCount; n++)
	{
		int a = strcmp(meshArray[n].name, name);
		if (a == 0) return n;
	}
	// if we're still here that means the shader doesn't exist. proceed in loading

	MeshObj temp;

	char * ext;
	char filename[1024];
	strcpy(filename, name);

	ext = strtok(filename, ".");
	ext = strtok(NULL, ".");


	if ( strcmp(ext, "obj") == 0 )
	{
		bool exist = false;

		MeshFileData store;
		exist = loadOBJ(name, store);

		
		indexVBO(store.vertices, store.uvs, store.normals, store.colors, store.tangents, temp.indices, temp.ind_vertex, temp.ind_uv, temp.ind_normal, temp.ind_GI, temp.ind_tangent);
		if (temp.ind_GI.size > 0) {
			printf("\n\n\n\n lighting works \n\n\n\n\n");
			temp.useVertexLight = true;
		}
		//temp.ind_GI = Stack<glm::vec3>(temp.indices.size);
		temp.ind_tangent = Stack<glm::vec3>(temp.indices.size);
		temp.ind_binormal = Stack<glm::vec3>(temp.indices.size);
		temp.getTangents();


		GLuint VertexArrayID;
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		// This will identify our vertex buffer

		glGenBuffers(1, &temp.vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, temp.vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, temp.ind_vertex.size * sizeof(glm::vec3), &temp.ind_vertex.item[0], GL_STATIC_DRAW);

		//color buffer

		glGenBuffers(1, &temp.uvBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, temp.uvBuffer);
		glBufferData(GL_ARRAY_BUFFER, temp.ind_uv.size * sizeof(glm::vec2), &temp.ind_uv.item[0], GL_STATIC_DRAW);

		//normal buffer
		glGenBuffers(1, &temp.normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, temp.normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, temp.ind_normal.size * sizeof(glm::vec3), &temp.ind_normal.item[0], GL_STATIC_DRAW);

		//GI buffer
		glGenBuffers(1, &temp.vertGIBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, temp.vertGIBuffer);
		glBufferData(GL_ARRAY_BUFFER, temp.ind_GI.size * sizeof(glm::vec3), &temp.ind_GI.item[0], GL_STATIC_DRAW);

		//tangent buffer
		glGenBuffers(1, &temp.tangentBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, temp.tangentBuffer);
		glBufferData(GL_ARRAY_BUFFER, temp.ind_tangent.size * sizeof(glm::vec3), &temp.ind_tangent.item[0], GL_STATIC_DRAW);

		//binormal buffer
		glGenBuffers(1, &temp.binormalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, temp.binormalBuffer);
		glBufferData(GL_ARRAY_BUFFER, temp.ind_binormal.size * sizeof(glm::vec3), &temp.ind_binormal.item[0], GL_STATIC_DRAW);
		
		// Generate a buffer for the indices as well

		glGenBuffers(1, &temp.elementBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, temp.elementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, temp.indices.size * sizeof(unsigned short), &temp.indices.item[0], GL_STATIC_DRAW);

		temp.shaderIndex = defaultShader;// &shaderManager->shaderArray[defaultShader];
		temp.texture[0] = textureManager->texArray[defaultTexture].id;
		strcpy(temp.name, filename);

		temp.calcMinMax();

		temp.id = assignID();
		return add(temp);
	}
	else
		printf("not Obj file");

	return -1;
}

int MeshManager::createQuad()
{
	MeshObj temp;
	temp.vertexBuffer = quad_vertexbuffer;
	temp.type = QUAD;
	temp.texture[0] = defaultTexture;
	
	temp.id = assignID();
	return add(temp);
}

int MeshManager::createPlane(glm::vec2 size)
{

	Stack<glm::vec3> vertex;
	Stack<glm::vec2> uv;
	Stack<glm::vec3> normal;
	Stack<glm::vec3> color;
	Stack<glm::vec3> tangent;

	vertex.add( glm::vec3(.5*size.x, -.5*size.y, 0) );
	
	vertex.add(glm::vec3(.5*size.x, .5*size.y, 0));
	vertex.add(glm::vec3(-.5*size.x, .5*size.y, 0));
	vertex.add( glm::vec3(.5*size.x, -.5*size.y, 0) );
	
	vertex.add(glm::vec3(-.5*size.x, .5*size.y, 0));
	vertex.add(glm::vec3(-.5*size.x, -.5*size.y, 0));

	uv.add( glm::vec3(1,1 , 0) );
	uv.add(glm::vec3(1, 0, 0));
	uv.add( glm::vec3(0, 0, 0) );
	
	uv.add( glm::vec3(1, 1, 0) );
	uv.add(glm::vec3(0, 0, 0));
	uv.add( glm::vec3(0, 1, 0) );
	

	normal.add( glm::vec3(0, 0, 1) );
	normal.add( glm::vec3(0, 0, 1) );
	normal.add( glm::vec3(0, 0, 1) );
	normal.add( glm::vec3(0, 0, 1) );
	normal.add( glm::vec3(0, 0, 1) );
	normal.add( glm::vec3(0, 0, 1) );

	tangent.add(glm::vec3(1, 0, 0));
	tangent.add(glm::vec3(1, 0, 0));
	tangent.add(glm::vec3(1, 0, 0));
	tangent.add(glm::vec3(1, 0, 0));
	tangent.add(glm::vec3(1, 0, 0));
	tangent.add(glm::vec3(1, 0, 0));

	color.add(glm::vec3(1));
	color.add(glm::vec3(1));
	color.add(glm::vec3(1));
	color.add(glm::vec3(1));
	color.add(glm::vec3(1));
	color.add(glm::vec3(1));

	normal.add(glm::vec3(0, 0, 1));
	normal.add(glm::vec3(0, 0, 1));
	normal.add(glm::vec3(0, 0, 1));
	normal.add(glm::vec3(0, 0, 1));
	normal.add(glm::vec3(0, 0, 1));
	normal.add(glm::vec3(0, 0, 1));

	MeshObj temp;
	//temp.name = "gen plane";
	strcpy(temp.name, "gen plane");

	indexVBO(vertex, uv, normal, color, tangent, temp.indices, temp.ind_vertex, temp.ind_uv, temp.ind_normal, temp.ind_GI, temp.ind_tangent);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// This will identify our vertex buffer

	glGenBuffers(1, &temp.vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, temp.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, temp.ind_vertex.size * sizeof(glm::vec3), &temp.ind_vertex.item[0], GL_STATIC_DRAW);

	//uv buffer

	glGenBuffers(1, &temp.uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, temp.uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, temp.ind_uv.size * sizeof(glm::vec2), &temp.ind_uv.item[0], GL_STATIC_DRAW);

	//normal buffer

	glGenBuffers(1, &temp.normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, temp.normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, temp.ind_normal.size * sizeof(glm::vec3), &temp.ind_normal.item[0], GL_STATIC_DRAW);

	//GI buffer
	glGenBuffers(1, &temp.vertGIBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, temp.vertGIBuffer);
	glBufferData(GL_ARRAY_BUFFER, temp.ind_GI.size * sizeof(glm::vec3), &temp.ind_GI.item[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well

	glGenBuffers(1, &temp.elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, temp.elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, temp.indices.size * sizeof(unsigned short), &temp.indices.item[0], GL_STATIC_DRAW);

	temp.shaderIndex = defaultShader; // &shaderManager->shaderArray[defaultShader];
	temp.texture[0] = textureManager->texArray[defaultTexture].id;
	
	temp.calcMinMax();

	temp.id = assignID();
	return add(temp);


}

int MeshManager::createLine3(glm::vec3 p0, glm::vec3 p1, float weight)
{
	Stack<glm::vec3> vertex;
	Stack<glm::vec2> uv;
	Stack<glm::vec3> normal;
	Stack<glm::vec3> color;
	Stack<glm::vec3> tangent;

	float size = glm::length(p1 - p0);
	
	//plane 1
	vertex.add(glm::vec3(-.5*weight, 0, 0));
	vertex.add(glm::vec3(-.5*weight, 0, 1));
	vertex.add(glm::vec3(.5*weight, 0, 0));

	vertex.add(glm::vec3(.5*weight, 0, 0));
	vertex.add(glm::vec3(-.5*weight, 0, 1));
	vertex.add(glm::vec3(.5*weight, 0, 1));

	uv.add(glm::vec3(0, 0, 0));
	uv.add(glm::vec3(0, 1, 0));
	uv.add(glm::vec3(1, 0, 0));

	uv.add(glm::vec3(1, 0, 0));
	uv.add(glm::vec3(0, 1, 0));
	uv.add(glm::vec3(1, 1, 0));

	normal.add(glm::vec3(0, 1, 0));
	normal.add(glm::vec3(0, 1, 0));
	normal.add(glm::vec3(0, 1, 0));
	normal.add(glm::vec3(0, 1, 0));
	normal.add(glm::vec3(0, 1, 0));
	normal.add(glm::vec3(0, 1, 0));

	color.add(glm::vec3(1));
	color.add(glm::vec3(1));
	color.add(glm::vec3(1));
	color.add(glm::vec3(1));
	color.add(glm::vec3(1));
	color.add(glm::vec3(1));

	// plane 2
	vertex.add(glm::vec3(0, -.5*weight, 0));
	vertex.add(glm::vec3(0, -.5*weight, 1));
	vertex.add(glm::vec3(0, .5*weight, 0));

	vertex.add(glm::vec3(0, .5*weight, 0));
	vertex.add(glm::vec3(0, -.5*weight, 1));
	vertex.add(glm::vec3(0, .5*weight, 1));

	uv.add(glm::vec3(0, 0, 0));
	uv.add(glm::vec3(0, 1, 0));
	uv.add(glm::vec3(1, 0, 0));

	uv.add(glm::vec3(1, 0, 0));
	uv.add(glm::vec3(0, 1, 0));
	uv.add(glm::vec3(1, 1, 0));

	normal.add(glm::vec3(1, 0, 0));
	normal.add(glm::vec3(1, 0, 0));
	normal.add(glm::vec3(1, 0, 0));
	normal.add(glm::vec3(1, 0, 0));
	normal.add(glm::vec3(1, 0, 0));
	normal.add(glm::vec3(1, 0, 0));


	MeshObj temp;
	//temp.name = "gen plane";
	strcpy(temp.name, "gen line");

	indexVBO(vertex, uv, normal, color, tangent, temp.indices, temp.ind_vertex, temp.ind_uv, temp.ind_normal, temp.ind_GI, temp.ind_tangent);

	temp.pos = p0;
	temp.scale = glm::vec3(1, 1, size);
	temp.lookat( p1, 50);
	temp.modelMatrix = temp.updateModelMatrix(temp.transformMatrix);
	
	temp.castShadow = false;
	
	temp.culling = 0;

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// This will identify our vertex buffer

	glGenBuffers(1, &temp.vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, temp.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, temp.ind_vertex.size * sizeof(glm::vec3), &temp.ind_vertex.item[0], GL_STATIC_DRAW);

	//uv buffer

	glGenBuffers(1, &temp.uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, temp.uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, temp.ind_uv.size * sizeof(glm::vec2), &temp.ind_uv.item[0], GL_STATIC_DRAW);

	//normal buffer

	glGenBuffers(1, &temp.normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, temp.normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, temp.ind_normal.size * sizeof(glm::vec3), &temp.ind_normal.item[0], GL_STATIC_DRAW);

	//GI buffer
	glGenBuffers(1, &temp.vertGIBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, temp.vertGIBuffer);
	glBufferData(GL_ARRAY_BUFFER, temp.ind_GI.size * sizeof(glm::vec3), &temp.ind_GI.item[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well

	glGenBuffers(1, &temp.elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, temp.elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, temp.indices.size * sizeof(unsigned short), &temp.indices.item[0], GL_STATIC_DRAW);

	Shader *shader = &shaderManager->shaderArray[lineShader];

	temp.shaderIndex = lineShader;
	temp.transparent = false;
	temp.setShaderConst(shader, "vcolor", 1.0, 1.0, 1.0, 1.0);
	temp.calcMinMax();

	temp.id = assignID();
	return add(temp);

}

bool MeshManager::raycastMesh(int id, glm::vec3 p0, glm::vec3 p1)
{
	raycastHitCount = 0;

	if (p0.x < meshArray[id].minpos.x && p1.x < meshArray[id].minpos.x) return false;
	if (p0.x > meshArray[id].maxpos.x && p1.x > meshArray[id].maxpos.x) return false;
	if (p0.y < meshArray[id].minpos.y && p1.y < meshArray[id].minpos.y) return false;
	if (p0.y > meshArray[id].maxpos.y && p1.y > meshArray[id].maxpos.y) return false;
	if (p0.z < meshArray[id].minpos.z && p1.z < meshArray[id].minpos.z) return false;
	if (p0.z > meshArray[id].maxpos.z && p1.z > meshArray[id].maxpos.z) return false;

	indexCount = meshArray[id].indices.size;

	for (int n = 0; n < indexCount ; n += 3)
	{
		
		raycast_index[0] = meshArray[id].indices.item[n];
		raycast_index[1] = meshArray[id].indices.item[n+1];
		raycast_index[2] = meshArray[id].indices.item[n+2];
		
		raycast_vert[0] = meshArray[id].ind_vertex.item[raycast_index[0]];
		raycast_vert[1] = meshArray[id].ind_vertex.item[raycast_index[1]];
		raycast_vert[2] = meshArray[id].ind_vertex.item[raycast_index[2]];
		

		raycastTriangle(p0, p1, raycast_vert[0], raycast_vert[1], raycast_vert[2]);
	}

	if (raycastHitCount > 0) return true;
	
	return false;
}

bool MeshManager::raycastAllMesh( glm::vec3 p0, glm::vec3 p1)
{
	raycastHitCount = 0;

	for (int id = 0; id < meshCount; id++)
	{
		glm::vec3 offset = -meshArray[id].pos;
		glm::vec3 scale = 1.0f / meshArray[id].scale;
		glm::vec3 rad_rot = -glm::radians(meshArray[id].rot);

		glm::mat4 scaling = glm::mat4(
			scale.x, 0.0, 0.0, 0.0,
			0.0, scale.y, 0.0, 0.0,
			0.0, 0.0, scale.z, 0.0,
			0.0, 0.0, 0.0, 1.0);

		glm::mat4 translating = glm::mat4(
			1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			offset.x, offset.y, offset.z, 1.0);

		glm::mat4 xrotation = glm::mat4(
			1.0, 0.0, 0.0, 0.0,
			0.0, cos(rad_rot.x), sin(rad_rot.x), 0.0,
			0.0, -sin(rad_rot.x), cos(rad_rot.x), 0.0,
			0.0, 0.0, 0.0, 1.0);

		glm::mat4 yrotation = glm::mat4(
			cos(rad_rot.y), 0.0, -sin(rad_rot.y), 0.0,
			0.0, 1.0, 0.0, 0.0,
			sin(rad_rot.y), 0.0, cos(rad_rot.y), 0.0,
			0.0, 0.0, 0.0, 1.0);

		glm::mat4 zrotation = glm::mat4(
			cos(rad_rot.z), sin(rad_rot.z), 0.0, 0.0,
			-sin(rad_rot.z), cos(rad_rot.z), 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0);

		glm::mat4 transform = scaling *xrotation*yrotation*zrotation *translating ;
		//transform = glm::inverse(transform);

		glm::vec4 np0 = transform* glm::vec4(p0, 1);
		glm::vec4 np1 = transform* glm::vec4(p1, 1);

		glm::vec3 startPos = glm::vec3(np0.x, np0.y, np0.z);
		glm::vec3 endPos = glm::vec3(np1.x, np1.y, np1.z);

		if (meshArray[id].collision)
		{
			
			if (startPos.x < meshArray[id].minpos.x && endPos.x < meshArray[id].minpos.x) continue;
			if (startPos.x > meshArray[id].maxpos.x && endPos.x > meshArray[id].maxpos.x) continue;
			if (startPos.y < meshArray[id].minpos.y && endPos.y < meshArray[id].minpos.y) continue;
			if (startPos.y > meshArray[id].maxpos.y && endPos.y > meshArray[id].maxpos.y) continue;
			if (startPos.z < meshArray[id].minpos.z && endPos.z < meshArray[id].minpos.z) continue;
			if (startPos.z > meshArray[id].maxpos.z && endPos.z > meshArray[id].maxpos.z) continue;
			
			indexCount = meshArray[id].indices.size;

			for (int n = 0; n < indexCount; n += 3)
			{

				raycast_index[0] = meshArray[id].indices.item[n];
				raycast_index[1] = meshArray[id].indices.item[n + 1];
				raycast_index[2] = meshArray[id].indices.item[n + 2];

				//printf("%d \n", n);

				raycast_vert[0] = meshArray[id].ind_vertex.item[raycast_index[0]];
				raycast_vert[1] = meshArray[id].ind_vertex.item[raycast_index[1]];
				raycast_vert[2] = meshArray[id].ind_vertex.item[raycast_index[2]];


				if (raycastTriangle(startPos, endPos, raycast_vert[0], raycast_vert[1], raycast_vert[2]))
				{
					raycastDataArray[raycastHitCount - 1].meshInd = id;
					raycastDataArray[raycastHitCount - 1].pos = meshArray[id].modelMatrix * glm::vec4(raycastDataArray[raycastHitCount - 1].pos, 1);
					raycastDataArray[raycastHitCount - 1].raylength = glm::length(raycastDataArray[raycastHitCount - 1].pos - p0);
				}
			}
		}
	}


	if (raycastHitCount > 0) return true;

	return false;
}

bool MeshManager::raycastAllMeshIgnoreStates(glm::vec3 p0, glm::vec3 p1)
{
	raycastHitCount = 0;

	for (int id = 0; id < meshCount; id++)
	{
		if (meshArray[id].collision && meshArray[id].type != EDITOR)
		{
			glm::vec3 offset = -meshArray[id].pos;
			glm::vec3 scale = 1.0f/meshArray[id].scale;
			glm::vec3 rad_rot = -glm::radians(meshArray[id].rot);

			glm::mat4 scaling = glm::mat4(
				scale.x, 0.0, 0.0, 0.0,
				0.0, scale.y, 0.0, 0.0,
				0.0, 0.0, scale.z, 0.0,
				0.0, 0.0, 0.0, 1.0);

			glm::mat4 translating = glm::mat4(
				1.0, 0.0, 0.0, 0.0,
				0.0, 1.0, 0.0, 0.0,
				0.0, 0.0, 1.0, 0.0,
				offset.x, offset.y, offset.z, 1.0);

			glm::mat4 xrotation = glm::mat4(
				1.0, 0.0, 0.0, 0.0,
				0.0, cos(rad_rot.x), sin(rad_rot.x), 0.0,
				0.0, -sin(rad_rot.x), cos(rad_rot.x), 0.0,
				0.0, 0.0, 0.0, 1.0);

			glm::mat4 yrotation = glm::mat4(
				cos(rad_rot.y), 0.0, -sin(rad_rot.y), 0.0,
				0.0, 1.0, 0.0, 0.0,
				sin(rad_rot.y), 0.0, cos(rad_rot.y), 0.0,
				0.0, 0.0, 0.0, 1.0);

			glm::mat4 zrotation = glm::mat4(
				cos(rad_rot.z), sin(rad_rot.z), 0.0, 0.0,
				-sin(rad_rot.z), cos(rad_rot.z), 0.0, 0.0,
				0.0, 0.0, 1.0, 0.0,
				0.0, 0.0, 0.0, 1.0);

			glm::mat4 transform = scaling*xrotation*yrotation*zrotation* translating;
			//transform = glm::inverse(transform);

			glm::vec4 np0 = transform* glm::vec4(p0, 1);
			glm::vec4 np1 = transform* glm::vec4(p1, 1);

			glm::vec3 startPos = glm::vec3(np0.x, np0.y, np0.z) ;
			glm::vec3 endPos = glm::vec3(np1.x, np1.y, np1.z);

			if (startPos.x < meshArray[id].minpos.x && endPos.x < meshArray[id].minpos.x) continue;
			if (startPos.x > meshArray[id].maxpos.x && endPos.x > meshArray[id].maxpos.x) continue;
			if (startPos.y < meshArray[id].minpos.y && endPos.y < meshArray[id].minpos.y) continue;
			if (startPos.y > meshArray[id].maxpos.y && endPos.y > meshArray[id].maxpos.y) continue;
			if (startPos.z < meshArray[id].minpos.z && endPos.z < meshArray[id].minpos.z) continue;
			if (startPos.z > meshArray[id].maxpos.z && endPos.z > meshArray[id].maxpos.z) continue;

			indexCount = meshArray[id].indices.size;

			for (int n = 0; n < indexCount; n += 3)
			{

				raycast_index[0] = meshArray[id].indices.item[n];
				raycast_index[1] = meshArray[id].indices.item[n + 1];
				raycast_index[2] = meshArray[id].indices.item[n + 2];

				raycast_vert[0] = meshArray[id].ind_vertex.item[raycast_index[0]];
				raycast_vert[1] = meshArray[id].ind_vertex.item[raycast_index[1]];
				raycast_vert[2] = meshArray[id].ind_vertex.item[raycast_index[2]];


				if (raycastTriangle(startPos, endPos, raycast_vert[0], raycast_vert[1], raycast_vert[2]))
				{	
					//printf("%f -> ", raycastDataArray[raycastHitCount - 1].pos.y);
					raycastDataArray[raycastHitCount - 1].meshInd = id;
					raycastDataArray[raycastHitCount - 1].pos = meshArray[id].modelMatrix * glm::vec4(raycastDataArray[raycastHitCount - 1].pos, 1);
					raycastDataArray[raycastHitCount - 1].raylength = glm::length(raycastDataArray[raycastHitCount - 1].pos -p0);
					
				}
			}
		}
	}

	if (raycastHitCount > 0) return true;

	return false;
}

bool MeshManager::spherecastMesh(int id, glm::vec3 center, float radius)
{
	spherecastHitCount = 0;



	if (center.x - radius < meshArray[id].minpos.x && center.x + radius < meshArray[id].minpos.x) return false;
	if (center.x - radius > meshArray[id].maxpos.x && center.x + radius > meshArray[id].maxpos.x) return false;
	if (center.y - radius < meshArray[id].minpos.y && center.y + radius < meshArray[id].minpos.y) return false;
	if (center.y - radius > meshArray[id].maxpos.y && center.y + radius > meshArray[id].maxpos.y) return false;
	if (center.z - radius < meshArray[id].minpos.z && center.z + radius < meshArray[id].minpos.z) return false;
	if (center.z - radius > meshArray[id].maxpos.z && center.z + radius > meshArray[id].maxpos.z) return false;

	indexCount = meshArray[id].indices.size;
	sampleSphereCenter = center;
	sampleSphereRadius = radius;

	for (int n = 0; n < indexCount; n += 3)
	{
		raycast_index[0] = meshArray[id].indices.item[n];
		raycast_index[1] = meshArray[id].indices.item[n + 1];
		raycast_index[2] = meshArray[id].indices.item[n + 2];

		raycast_vert[0] = meshArray[id].ind_vertex.item[raycast_index[0]];
		raycast_vert[1] = meshArray[id].ind_vertex.item[raycast_index[1]];
		raycast_vert[2] = meshArray[id].ind_vertex.item[raycast_index[2]];

		spherecastTriangle(center, radius, raycast_vert[0], raycast_vert[1], raycast_vert[2]);
	}

	if (spherecastHitCount > 0)
	{
		return true;
	}
	return false;
}

bool MeshManager::spherecastAllMesh(glm::vec3 center, float radius)
{
	spherecastHitCount = 0;
	for (int id = 0; id < meshCount; id++)
	{

		glm::vec3 offset = -meshArray[id].pos;
		glm::vec3 scale = 1.0f / meshArray[id].scale;
		glm::vec3 rad_rot = -glm::radians(meshArray[id].rot);

		glm::mat4 scaling = glm::mat4(
			scale.x, 0.0, 0.0, 0.0,
			0.0, scale.y, 0.0, 0.0,
			0.0, 0.0, scale.z, 0.0,
			0.0, 0.0, 0.0, 1.0);

		glm::mat4 translating = glm::mat4(
			1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			offset.x, offset.y, offset.z, 1.0);

		glm::mat4 xrotation = glm::mat4(
			1.0, 0.0, 0.0, 0.0,
			0.0, cos(rad_rot.x), sin(rad_rot.x), 0.0,
			0.0, -sin(rad_rot.x), cos(rad_rot.x), 0.0,
			0.0, 0.0, 0.0, 1.0);

		glm::mat4 yrotation = glm::mat4(
			cos(rad_rot.y), 0.0, -sin(rad_rot.y), 0.0,
			0.0, 1.0, 0.0, 0.0,
			sin(rad_rot.y), 0.0, cos(rad_rot.y), 0.0,
			0.0, 0.0, 0.0, 1.0);

		glm::mat4 zrotation = glm::mat4(
			cos(rad_rot.z), sin(rad_rot.z), 0.0, 0.0,
			-sin(rad_rot.z), cos(rad_rot.z), 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0);

		glm::mat4 transform = scaling * xrotation*yrotation*zrotation* translating;
		//transform = glm::inverse(transform);

		glm::vec4 np0 = transform * glm::vec4(center, 1);
		glm::vec3 newcenter = glm::vec3(np0.x, np0.y, np0.z);

		bool hit;

		if (meshArray[id].collision)
		{
			if (newcenter.x - radius < meshArray[id].minpos.x && newcenter.x + radius < meshArray[id].minpos.x) continue;
			if (newcenter.x - radius > meshArray[id].maxpos.x && newcenter.x + radius > meshArray[id].maxpos.x) continue;
			if (newcenter.y - radius < meshArray[id].minpos.y && newcenter.y + radius < meshArray[id].minpos.y) continue;
			if (newcenter.y - radius > meshArray[id].maxpos.y && newcenter.y + radius > meshArray[id].maxpos.y) continue;
			if (newcenter.z - radius < meshArray[id].minpos.z && newcenter.z + radius < meshArray[id].minpos.z) continue;
			if (newcenter.z - radius > meshArray[id].maxpos.z && newcenter.z + radius > meshArray[id].maxpos.z) continue;

			indexCount = meshArray[id].indices.size;
			sampleSphereCenter = center;
			sampleSphereRadius = radius;

			for (int n = 0; n < indexCount; n += 3)
			{
				raycast_index[0] = meshArray[id].indices.item[n];
				raycast_index[1] = meshArray[id].indices.item[n + 1];
				raycast_index[2] = meshArray[id].indices.item[n + 2];

				raycast_vert[0] = meshArray[id].ind_vertex.item[raycast_index[0]];
				raycast_vert[1] = meshArray[id].ind_vertex.item[raycast_index[1]];
				raycast_vert[2] = meshArray[id].ind_vertex.item[raycast_index[2]];

				if (spherecastTriangle(newcenter, radius, raycast_vert[0], raycast_vert[1], raycast_vert[2]))
				{
					printf("%f ->", spherecastDataArray[spherecastHitCount - 1].pos.x);
					spherecastDataArray[spherecastHitCount - 1].pos = meshArray[id].modelMatrix * glm::vec4(spherecastDataArray[spherecastHitCount - 1].pos, 1);
					spherecastDataArray[spherecastHitCount - 1].norm = glm::normalize(meshArray[id].transformMatrix * glm::vec4(spherecastDataArray[spherecastHitCount - 1].norm, 1));
					spherecastDataArray[spherecastHitCount - 1].raylength = glm::length(spherecastDataArray[spherecastHitCount - 1].pos - center);
					printf("%f \n", spherecastDataArray[spherecastHitCount - 1].raylength);
					printf("%f \n", spherecastDataArray[spherecastHitCount - 1].pos.x);
				}
			}
		}
	}
	if (spherecastHitCount > 0)
	{
		return true;
	}
	return false;
}

bool MeshManager::raycastTriangle(glm::vec3 p0, glm::vec3 p1, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
{
	// do box vetting test to make sure we're not 
	// raycasting against a triangle that isn't in range
	
	float minx, maxx;
	minx = v0.x;
	if (minx > v1.x) minx = v1.x;
	if (minx > v2.x) minx = v2.x;
	if (p0.x < minx && p1.x < minx) return false;
	maxx = v0.x;
	if (maxx < v1.x) maxx = v1.x;
	if (maxx < v2.x) maxx = v2.x;
	if (p0.x > maxx && p1.x > maxx) return false;
	
	float miny, maxy;
	miny = v0.y;
	if (miny > v1.y) miny = v1.y;
	if (miny > v2.y) miny = v2.y;
	if (p0.y < miny && p1.y < miny) return false;
	maxy = v0.y;
	if (maxy < v1.y) maxy = v1.y;
	if (maxy < v2.y) maxy = v2.y;
	if (p0.y > maxy && p1.y > maxy) return false;
	
	float minz, maxz;
	minz = v0.z;
	if (minz > v1.z) minz = v1.z;
	if (minz > v2.z) minz = v2.z;
	if (p0.z < minz && p1.z < minz) return false;
	maxz = v0.z;
	if (maxz < v1.z) maxz = v1.z;
	if (maxz < v2.z) maxz = v2.z;
	if (p0.z > maxz && p1.z > maxz) return false;
	
	
	raydir = glm::normalize(p1 - p0);
	raylength = glm::length(p1 - p0);


	glm::vec3 norm = glm::cross(v1 - v0, v2 - v0);

	//
	dotNormRaydir = glm::dot(raydir, norm);

	// if angle between ray and normal less than or equal to pi/2
	// raycast will fail, as we don't count back-face collisions
	if (dotNormRaydir >= 0.0) return false;

	// return false if ray is coplanar with triangle
	if ( abs(dotNormRaydir) < 0.0001) return false;

	// D component
	float planeOffset = glm::dot(norm, v0);

	float t = ( glm::dot(-p0, norm) + planeOffset ) / (dotNormRaydir);

	// triangle out of range
	if (t > raylength) return false;
	// ray occurs behind triangle
	if (t < 0.0) return false;

	glm::vec3 intersectionPoint = p0 + raydir * t;

	glm::vec3 edge = v1 - v0;
	glm::vec3 vp0 = intersectionPoint - v0;
	glm::vec3 c = glm::cross(edge, vp0);
	if (glm::dot(c, norm) < 0.0) return false;

	edge = v2 - v1;
	vp0 = intersectionPoint - v1;
	c = glm::cross(edge, vp0);
	if (glm::dot(c, norm) < 0.0) return false;

	edge = v0 - v2;
	vp0 = intersectionPoint - v2;
	c = glm::cross(edge, vp0);
	if (glm::dot(c, norm) < 0.0) return false;

	if (raycastHitCount < 30)
	{
		raycastDataArray[raycastHitCount].raylength = t;
		raycastDataArray[raycastHitCount].pos = intersectionPoint;
		raycastDataArray[raycastHitCount].norm = glm::normalize(norm);
		raycastHitCount++;
	}
	return true;
}

// determine if and where a sphere intersects a triangle
bool MeshManager::spherecastTriangle(glm::vec3 center, float radius, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
{
	// do box vetting test to make sure we're not 
	// raycasting against a triangle that isn't in range

	float minx, maxx;
	minx = v0.x;
	if (minx > v1.x) minx = v1.x;
	if (minx > v2.x) minx = v2.x;
	if (center.x-radius < minx && center.x + radius < minx) return false;
	maxx = v0.x;
	if (maxx < v1.x) maxx = v1.x;
	if (maxx < v2.x) maxx = v2.x;
	if (center.x - radius > maxx && center.x + radius > maxx) return false;

	float miny, maxy;
	miny = v0.y;
	if (miny > v1.y) miny = v1.y;
	if (miny > v2.y) miny = v2.y;
	if (center.y - radius < miny && center.y + radius < miny) return false;
	maxy = v0.y;
	if (maxy < v1.y) maxy = v1.y;
	if (maxy < v2.y) maxy = v2.y;
	if (center.y - radius > maxy && center.y + radius > maxy) return false;

	float minz, maxz;
	minz = v0.z;
	if (minz > v1.z) minz = v1.z;
	if (minz > v2.z) minz = v2.z;
	if (center.z - radius < minz && center.z + radius < minz) return false;
	maxz = v0.z;
	if (maxz < v1.z) maxz = v1.z;
	if (maxz < v2.z) maxz = v2.z;
	if (center.z - radius > maxz && center.z + radius > maxz) return false;

	glm::vec3 norm = glm::cross(v1 - v0, v2 - v0);

	raydir = glm::normalize(-norm);
	raylength = radius;

	//
	dotNormRaydir = glm::dot(raydir, norm);

	// D component
	float planeOffset = glm::dot(norm, v0);

	float t = (glm::dot(-center, norm) + planeOffset) / (dotNormRaydir);

	// triangle out of range
	if (t > raylength) return false;
	// ray occurs behind triangle
	if (t < 0.0) return false;

	glm::vec3 intersectionPoint = center + raydir * t;

	bool surfaceHit = false;

	do
	{
		glm::vec3 edge = v1 - v0;
		glm::vec3 vp0 = intersectionPoint - v0;
		glm::vec3 c = glm::cross(edge, vp0);
		if (glm::dot(c, norm) < 0.0) break;

		edge = v2 - v1;
		vp0 = intersectionPoint - v1;
		c = glm::cross(edge, vp0);
		if (glm::dot(c, norm) < 0.0) break;

		edge = v0 - v2;
		vp0 = intersectionPoint - v2;
		c = glm::cross(edge, vp0);
		if (glm::dot(c, norm) < 0.0) break;

		surfaceHit = true;

		
	} while (1 == 0);
	
	if (surfaceHit)
	{
		spherecastDataArray[spherecastHitCount].raylength = t;
		spherecastDataArray[spherecastHitCount].pos = intersectionPoint;
		spherecastDataArray[spherecastHitCount].norm = glm::normalize(norm);
		spherecastHitCount++;
		
		return true;
	}
	
	// if the sphere does hit the surface of triange, it may hit the edge

	raydir = glm::normalize(v1 - v0);
	norm = -raydir;

	planeOffset = glm::dot(norm, center);
	t = (glm::dot(-v0, norm) + planeOffset) / -1.0;

	intersectionPoint = v0 + raydir*t;

	float dist = glm::length(intersectionPoint - center);
	
	if (dist <= radius)
	{
		spherecastDataArray[spherecastHitCount].raylength = dist;
		spherecastDataArray[spherecastHitCount].pos = intersectionPoint;
		spherecastDataArray[spherecastHitCount].norm = glm::normalize(center - intersectionPoint);
		spherecastHitCount++;
		return true;
	}

	raydir = glm::normalize(v2 - v0);
	norm = -raydir;

	planeOffset = glm::dot(norm, center);
	t = ( glm::dot(-v0, norm) + planeOffset ) / -1.0;

	intersectionPoint = v0 + raydir*t;

	dist = glm::length(intersectionPoint - center);
	
	if (dist <= radius)
	{
		spherecastDataArray[spherecastHitCount].raylength = dist;
		spherecastDataArray[spherecastHitCount].pos = intersectionPoint;
		spherecastDataArray[spherecastHitCount].norm = glm::normalize(center - intersectionPoint);
		spherecastHitCount++;
		return true;
	}

	raydir = glm::normalize(v2 - v1);
	norm = -raydir;

	planeOffset = glm::dot(norm, center);
	t = (glm::dot(-v1, norm) + planeOffset) / -1.0;

	intersectionPoint = v1 + raydir*t;

	dist = glm::length(intersectionPoint - center);

	if (dist <= radius)
	{
		spherecastDataArray[spherecastHitCount].raylength = dist;
		spherecastDataArray[spherecastHitCount].pos = intersectionPoint;
		spherecastDataArray[spherecastHitCount].norm = glm::normalize(center - intersectionPoint);
		spherecastHitCount++;

		return true;
	}

	return false;
}

glm::vec3 MeshManager::resolveSphereSlide()
{
	if (spherecastHitCount > 1)
	{
		sortSphereCastData();
	}


	printf("before %f  \n", sampleSphereCenter.x);


	float dist = sampleSphereRadius - spherecastDataArray[0].raylength;

	sampleSphereCenter += dist * spherecastDataArray[0].norm;



	printf("after %f  \n", sampleSphereCenter.x);
	
	return sampleSphereCenter;
}

void MeshManager::sortRayCastData()
{
	sortingArray = raycastDataArray;
	sortingHitCount = raycastHitCount;
	recQuickSort(0, sortingHitCount - 1);
}

void MeshManager::sortSphereCastData()
{
	sortingArray = spherecastDataArray;
	sortingHitCount = spherecastHitCount;
	recQuickSort(0, sortingHitCount - 1);
}

void MeshManager::recQuickSort(int left, int right)
{
	if (right - left <= 0)
		return;
	else
	{
		CollisionData pivot = sortingArray[right];

		int partition = partitionIt(left, right, pivot);
		recQuickSort(left, partition - 1);
		recQuickSort(partition + 1, right);
	}
}

int MeshManager::partitionIt(int left, int right, CollisionData pivot)
{
	int leftPtr = left - 1;
	int rightPtr = right;
	while (true)
	{
		while (sortingArray[++leftPtr].raylength < pivot.raylength)
			;
		while (rightPtr > 0 && sortingArray[--rightPtr].raylength > pivot.raylength)
			;
		if (leftPtr >= rightPtr)
			break;
		else
			swap(leftPtr, rightPtr);
	}
	swap(leftPtr, right);
	return leftPtr;
}

void MeshManager::swap(int dex1, int dex2)
{
	CollisionData temp = sortingArray[dex1];
	sortingArray[dex1] = sortingArray[dex2];
	sortingArray[dex2] = temp;
}

void MeshManager::setShader(unsigned int meshIndex, unsigned int shaderIndex)
{
	meshArray[meshIndex].shaderIndex = shaderIndex;

	return;
}

void MeshManager::setTexture(int index, int texID, int texIndex)
{
	meshArray[index].texture[texIndex] = textureManager->texArray[texID].id;
}

void MeshManager::setPosition(int index, glm::vec3 pos)
{
	meshArray[index].pos = pos;
}

void MeshManager::setRotation(int index, glm::vec3 rot)
{
	meshArray[index].rot = rot;
}

void MeshManager::setScale(int index, glm::vec3 scale)
{
	meshArray[index].scale = scale;
}

void MeshManager::setCollision(int index, bool state)
{
	meshArray[index].collision = state;
}

void MeshManager::setVisible(int index, bool state)
{
	meshArray[index].visible = state;
}

// 0 = none, 1 = backface, 2 = frontface
void MeshManager::setCullMode(int index, char state)
{
	meshArray[index].culling = state;
}

void MeshManager::setCastShadow(int index, bool state)
{
	meshArray[index].castShadow = state;
}

void MeshManager::setIsWater(int index, bool state)
{
	meshArray[index].isWater = state;
}

void MeshManager::setReflectable(int index, bool state)
{
	meshArray[index].reflectable = state;
}

void MeshManager::setFollowCam(int index, bool state)
{
	meshArray[index].follow = state;
}

void MeshManager::setTransparent(int index, bool state)
{
	meshArray[index].transparent = state;
}

void MeshManager::setLookatCam(int index, bool state)
{
	meshArray[index].lookatCam = state;
}

int MeshManager::getTexture(int index, int texIndex)
{
	return meshArray[index].texture[texIndex];
}

glm::vec3 MeshManager::getPosition(int index)
{
	return meshArray[index].pos;
}

glm::vec3 MeshManager::getRotation(int index)
{
	return meshArray[index].rot;
}

glm::vec3 MeshManager::getScale(int index)
{
	return meshArray[index].scale;
}

bool MeshManager::getCollision(int index)
{
	return meshArray[index].collision;
}

bool MeshManager::getVisible(int index)
{
	return meshArray[index].visible;
}

// 0 = none, 1 = backface, 2 = frontface
char MeshManager::getCullMode(int index) 
{
	return meshArray[index].culling;
}

bool MeshManager::getCastShadow(int index)
{
	return meshArray[index].castShadow;
}

bool MeshManager::getIsWater(int index)
{
	return meshArray[index].isWater;
}

bool MeshManager::getReflectable(int index)
{
	return meshArray[index].reflectable;
}

bool MeshManager::getFollowCam(int index)
{
	return meshArray[index].follow;
}

bool MeshManager::getTransparent(int index)
{
	return meshArray[index].transparent;
}

bool MeshManager::getLookatCam(int index)
{
	return meshArray[index].lookatCam;
}

int MeshManager::copy(int index)
{
	MeshObj temp = meshArray[index];

	//strcat(temp.name, "0");
	temp.pos += glm::vec3( .01*(rand()%100)-.5, .01*(rand() % 100) - .5, .01*(rand() % 100) - .5);
	temp.updateModelMatrix();

	temp.id = assignID();
	return add(temp);
}

void MeshManager::deleteMesh(int index)
{
	if (index == meshCount-1)
	{
		
		meshArray[index].indices.clean();
		meshArray[index].ind_vertex.clean();
		meshArray[index].ind_uv.clean();
		meshArray[index].ind_normal.clean();
		
		//track dependencies before deleting buffers
		/*
		glDeleteBuffers(1, &meshArray[index].vertexBuffer);
		glDeleteBuffers(1, &meshArray[index].elementBuffer);
		*/

		meshCount--;

		return;
	}

	if (index < meshCount)
	{
		meshArray[index].indices.clean();
		meshArray[index].ind_vertex.clean();
		meshArray[index].ind_uv.clean();
		meshArray[index].ind_normal.clean();
		
		//track dependencies before deleting buffers
		/*
		glDeleteBuffers(1, &meshArray[index].vertexBuffer);
		glDeleteBuffers(1, &meshArray[index].elementBuffer);
		*/

		meshArray[index] = meshArray[meshCount-1];
		meshCount--;

		return;
	}

}

int MeshManager::attach(int index0, int index1)
{
	MeshObj temp( meshArray[index0] );
	
	temp.indices.clean();
	temp.ind_vertex.clean();
	temp.ind_uv.clean();
	temp.ind_normal.clean();

	Stack<unsigned short> secondIndexSet(meshArray[index1].indices, true);
	Stack<glm::vec3> firstVertexSet( meshArray[index0].ind_vertex, true);
	Stack<glm::vec3> secondVertexSet(meshArray[index1].ind_vertex, true);

	// unify position attribute
	for (int n = 0; n < meshArray[index0].ind_vertex.size; n++)
	{
		firstVertexSet.item[n] += meshArray[index0].pos;
	}
	for (int n = 0; n < meshArray[index1].ind_vertex.size; n++)
	{
		secondVertexSet.item[n] += meshArray[index1].pos;
	}
	temp.pos = glm::vec3(0);


	for (int n = 0; n < meshArray[index1].indices.size; n++)
	{
		secondIndexSet.item[n] += meshArray[index0].ind_vertex.size;
	}
	
	temp.indices = Stack<unsigned short>::add(meshArray[index0].indices, secondIndexSet);
	temp.ind_vertex = Stack<glm::vec3>::add(firstVertexSet, secondVertexSet);
	temp.ind_uv = Stack<glm::vec2>::add(meshArray[index0].ind_uv, meshArray[index1].ind_uv);
	temp.ind_normal = Stack<glm::vec3>::add(meshArray[index0].ind_normal, meshArray[index1].ind_normal);


	temp.updateModelMatrix();

	// This will identify our vertex buffer
	glGenBuffers(1, &temp.vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, temp.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, temp.ind_vertex.size * sizeof(glm::vec3), &temp.ind_vertex.item[0], GL_STATIC_DRAW);

	//color buffer
	glGenBuffers(1, &temp.uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, temp.uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, temp.ind_uv.size * sizeof(glm::vec2), &temp.ind_uv.item[0], GL_STATIC_DRAW);

	//normal buffer
	glGenBuffers(1, &temp.normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, temp.normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, temp.ind_normal.size * sizeof(glm::vec3), &temp.ind_normal.item[0], GL_STATIC_DRAW);



	// Generate a buffer for the indices as well
	glGenBuffers(1, &temp.elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, temp.elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, temp.indices.size * sizeof(unsigned short), &temp.indices.item[0], GL_STATIC_DRAW);
	
	temp.id = assignID();
	return add(temp);
}

int MeshManager::assignID(int index)
{
	meshArray[index].id = lastGivenID + 1;

	return ++lastGivenID;
}

int MeshManager::assignID()
{
	return ++lastGivenID;
}

int MeshManager::getID(int index)
{
	return meshArray[index].id;
}

// return transform matrix only; no translation or scaling
glm::mat4 MeshManager::lookat(glm::vec3 pos, glm::vec3 lookatpos, float z_rotation)
{
	glm::vec3 lookvec = lookatpos - pos;
	float x_rotation = -atan2(lookvec.y, glm::length(glm::vec2(lookvec.x, lookvec.z)));// -3.14159;
	float y_rotation = atan2(lookvec.x, lookvec.z);

	

	glm::mat4 xrotation, yrotation, zrotation;
	xrotation = glm::mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, cos(x_rotation), sin(x_rotation), 0.0,
		0.0, -sin(x_rotation), cos(x_rotation), 0.0,
		0.0, 0.0, 0.0, 1.0);

	yrotation = glm::mat4(
		cos(y_rotation), 0.0, -sin(y_rotation), 0.0,
		0.0, 1.0, 0.0, 0.0,
		sin(y_rotation), 0.0, cos(y_rotation), 0.0,
		0.0, 0.0, 0.0, 1.0);

	zrotation = glm::mat4(
		cos(z_rotation), sin(z_rotation), 0.0, 0.0,
		-sin(z_rotation), cos(z_rotation), 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0);

	glm::mat4 fin;
	if (abs(z_rotation) > 0.0001)
		fin = yrotation*xrotation*zrotation; // this works perfectly
	else
		fin = yrotation*xrotation; // if no z_rotation specified, don't bother multiplying

	return fin;
}


// mesh specific: set internal rotation components and transform matrices
void MeshManager::lookat(int index, glm::vec3 pos, glm::vec3 lookatpos, float z_rotation)
{
	glm::vec3 lookvec = lookatpos - pos;
	float x_rotation = -atan2(lookvec.y, glm::length(glm::vec2(lookvec.x, lookvec.z)));// -3.14159;
	float y_rotation = atan2(lookvec.x, lookvec.z);



	glm::mat4 xrotation, yrotation, zrotation;
	xrotation = glm::mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, cos(x_rotation), sin(x_rotation), 0.0,
		0.0, -sin(x_rotation), cos(x_rotation), 0.0,
		0.0, 0.0, 0.0, 1.0);

	yrotation = glm::mat4(
		cos(y_rotation), 0.0, -sin(y_rotation), 0.0,
		0.0, 1.0, 0.0, 0.0,
		sin(y_rotation), 0.0, cos(y_rotation), 0.0,
		0.0, 0.0, 0.0, 1.0);

	zrotation = glm::mat4(
		cos(z_rotation), sin(z_rotation), 0.0, 0.0,
		-sin(z_rotation), cos(z_rotation), 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0);

	glm::mat4 fin;
	if (abs(z_rotation) > 0.0001)
		fin = yrotation * xrotation*zrotation; // this works perfectly
	else
		fin = yrotation * xrotation; // if no z_rotation specified, don't bother multiplying

	meshArray[index].transformMatrix = fin;
	meshArray[index].rot = glm::vec3(x_rotation, y_rotation, z_rotation);
}


void MeshManager::setShaderConst(int meshIndex, char *name, float v0, float v1, float v2, float v3)
{
	GLint loc;
	unsigned char type;

	Shader *shader = &shaderManager->shaderArray[meshArray[meshIndex].shaderIndex];
	
	loc = shader->getUniformLocation(name);
	type = shader->getUniformType(name);

	MeshObj *mesh = &meshArray[meshIndex];
	int MMconstantCount = mesh->constantCount;
	

	// check if uniform name already exists on this object
	for (int n = 0; n < MMconstantCount; n++)
	{
		if (meshArray[meshIndex].constantArray[n].name != NULL)
		{
			if (strcmp(name, meshArray[meshIndex].constantArray[n].name) == 0)
			{
				mesh->constantArray[n].v[0] = v0;
				mesh->constantArray[n].v[1] = v1;
				mesh->constantArray[n].v[2] = v2;
				mesh->constantArray[n].v[3] = v3;
				mesh->constantArray[n].location = loc;
				mesh->constantArray[n].s_type = type;

				return;
			}
		}
	}

	// if uniform name does not already exist... add it
	mesh->constantArray[MMconstantCount].name = name;
	mesh->constantArray[MMconstantCount].v[0] = v0;
	mesh->constantArray[MMconstantCount].v[1] = v1;
	mesh->constantArray[MMconstantCount].v[2] = v2;
	mesh->constantArray[MMconstantCount].v[3] = v3;
	mesh->constantArray[MMconstantCount].location = loc;
	mesh->constantArray[MMconstantCount].s_type = type;

	mesh->constantCount++;

	return;
}


void MeshManager::setShaderMat(int meshIndex, char *name, glm::mat4 val)
{
	GLint loc;

	Shader *shader = &shaderManager->shaderArray[meshArray[meshIndex].shaderIndex];

	loc = shader->getUniformLocation(name);

	MeshObj *mesh = &meshArray[meshIndex];
	int MMconstantCount = mesh->constantCount;

	// check if uniform name already exists on this object
	for (int n = 0; n < MMconstantCount; n++)
	{
		//if (constantMatArray[n].name != NULL)
		{
			if (strcmp(name, mesh->constantMatArray[n].name) == 0)
			{
				mesh->constantMatArray[n].value = val;
				mesh->constantMatArray[n].location = loc;

				return;
			}
		}
	}

	// if uniform name does not already exist... add it
	mesh->constantMatArray[MMconstantCount].name = name;
	mesh->constantMatArray[MMconstantCount].value = val;
	mesh->constantMatArray[MMconstantCount].location = loc;

	mesh->constantMatCount++;

	return;
}

void MeshManager::frustumCull(Camera cam, glm::mat4 proj, glm::vec2 res)
{
	
	glm::mat4 view = glm::lookAt(
		glm::vec3(0),
		cam.forward,
		cam.up
	);
	glm::mat4 view_projMatrixInverse = glm::inverse(proj * view);
	glm::vec4 rb = view_projMatrixInverse * glm::vec4(-1, 0, 0, 1);
	glm::vec4 lb = view_projMatrixInverse * glm::vec4(1, 0, 0, 1);
	

	glm::vec2 leftbound = glm::normalize(glm::vec2( lb.x, lb.z));
	glm::vec2 rightbound = glm::normalize(glm::vec2(rb.x, rb.z));

	//printf("%f , %f , %f , %f \n", leftbound.x, leftbound.y, rightbound.x, rightbound.y);

	//printf("(%f, %f) , (%f, %f) -> %f\n", leftbound.x, leftbound.y, rightbound.x, rightbound.y, glm::dot(leftbound, rightbound));
	float dotbounds = glm::dot(leftbound, rightbound);

	printf("%f  \n", dotbounds);

	for (int n = 0; n < meshCount; n++)
	{
		if (meshArray[n].type != QUAD && meshArray[n].type != EDITOR)
		{
			glm::vec3 maxpos = meshArray[n].maxpos * meshArray[n].scale + meshArray[n].pos;
			glm::vec3 minpos = meshArray[n].minpos * meshArray[n].scale + meshArray[n].pos;
			glm::vec3 vert[8];

			bool check[8];
			vert[0] = glm::vec3(maxpos.x, minpos.y, minpos.z);
			vert[1] = glm::vec3(maxpos.x, minpos.y, maxpos.z);
			vert[2] = glm::vec3(minpos.x, minpos.y, maxpos.z);
			vert[3] = glm::vec3(minpos.x, minpos.y, minpos.z);
			vert[4] = glm::vec3(maxpos.x, maxpos.y, minpos.z);
			vert[5] = glm::vec3(maxpos.x, maxpos.y, maxpos.z);
			vert[6] = glm::vec3(minpos.x, maxpos.y, maxpos.z);
			vert[7] = glm::vec3(minpos.x, maxpos.y, minpos.z);

			for (int c = 0; c < 8; c++)
			{
				check[c] = (glm::dot(cam.forward, glm::normalize(vert[c] - cam.pos)) > 0);
			}

			meshArray[n].visible = true;
			if (!check[0] && !check[1] && !check[2] && !check[3] && !check[4] && !check[5] && !check[6] && !check[7])
			{
				meshArray[n].visible = false;
				continue;
			}

			//printf("%s ", meshArray[n].name);
			// 0 = in frustum, 1 = left of, 2 = right of
			char within[8];
			for (int c = 0; c < 8; c++)
			{
				float dotleft = glm::dot(leftbound, glm::normalize(glm::vec2(vert[c].x - cam.pos.x, vert[c].z - cam.pos.z)) );
				float dotright = glm::dot(rightbound, glm::normalize(glm::vec2(vert[c].x - cam.pos.x, vert[c].z - cam.pos.z)));
				
				within[c] = -1;
				// left of view
				if (dotleft > dotbounds && dotright < dotbounds) within[c] = 1;
				// right of view
				if (dotleft < dotbounds && dotright > dotbounds) within[c] = 2;
				// within view
				if (dotleft > dotbounds && dotright > dotbounds) within[c] = 0;


				
				
				if (within[c] == -1)
				{
					if (dotleft < dotbounds && dotright < dotbounds)
					{
						if (dotleft < dotright)  within[c] = 2;
						if (dotleft >= dotright)  within[c] = 1;
					}
				}

				//printf(" %f %f ", vert[c].x, vert[c].z);
			}
			//printf("\n");
			if (within[0] == 1 && within[1] == 1 && within[2] == 1 && within[3] == 1 && within[4] == 1 && within[5] == 1 && within[6] == 1 && within[7] == 1)
			{
				meshArray[n].visible = false;
				continue;
			}
			if (within[0] == 2 && within[1] == 2 && within[2] == 2 && within[3] == 2 && within[4] == 2 && within[5] == 2 && within[6] == 2 && within[7] == 2)
			{
				meshArray[n].visible = false;
				continue;
			}
		}
	}
}