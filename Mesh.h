#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef MESH_OBJ_H_
#define MESH_OBJ_H_

struct VertexData;

#include "glm.hpp"
#include <glew.h>
#include "Shader.h"

#include "Texture.h"
#include "Camera.h"


#include "Octree.h"




namespace ENGINE
{

	struct MeshFileData
	{
		MeshFileData()
		{
			vertColors = false;
		}

		Stack < VertexData > vertexData;

		bool vertColors;
		glm::vec3 maxpos;
		glm::vec3 minpos;
	};

	struct PackedVertex
	{
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;
		glm::vec3 color;
		glm::vec3 tangent;
		glm::vec3 binormal;
		bool operator<(const PackedVertex that) const
		{
			return memcmp((void*)this, (void*)&that, sizeof(PackedVertex)) > 0;
		}
	};

	enum mesh_type
	{
		STANDARD,
		QUAD,
		EDITOR
	};

	struct MeshObj
	{
		MeshObj();
		MeshObj(const MeshObj &other);

		MeshObj instanceCopy();

		int id; // for tracking
		char name[1024];

		//indexed
		//std::vector<unsigned short> indices;
		Stack<unsigned int> indices;
		Stack<glm::vec3> ind_vertex;
		Stack<glm::vec2> ind_uv;
		Stack<glm::vec3> ind_normal;
		Stack<glm::vec3> ind_GI;
		Stack<glm::mat3> ind_grad;
		Stack<glm::vec3> ind_tangent;
		Stack<glm::vec3> ind_binormal;

		GLuint elementBuffer,
				vertexBuffer,
				uvBuffer,
				normalBuffer,
				vertGIBuffer,
				indirectGradBuffer, //indirect light directional gradients
				tangentBuffer,
				binormalBuffer;

		// properties
		bool transparent;
		char culling;
		bool depthWrite;
		char depthRead;
		bool visible;
		bool collision;
		bool castShadow;
		bool lookatCam;
		bool lookatPosState;
		bool follow;
		bool reflectable;
		bool isWater;
		char type;
		bool useVertexLight;
		char vertexLightBounces;
		bool useIndirectDirection;
		bool frustumCulled;
		unsigned int vertexCount;

		// 0 = not sampled, 1 = sampled & up to date, 2 = sampled & outdated
		char vertexLightSampleState;
		bool useTangents = false;

		glm::vec3 pos;
		glm::vec3 rot;
		glm::vec3 scale;
		glm::vec3 pivot;
		glm::vec3 lookatPos; // use if mesh is oriented at position
		glm::vec3 followPos; // use if mesh should follow camera

		// upper and lower bounds of mesh
		glm::vec3 minpos;
		glm::vec3 maxpos;

		int texIndex[24];
		GLint texture[24];

		//Shader shader;
		// keep position of shader in the case that shaders are remapped
		// to new memory locations
		unsigned short int shaderIndex;
		ShaderConst constantArray[32];
		int constantArraySize;
		int constantCount;

		ShaderMat constantMatArray[32];
		int constantMatCount;
		void setShaderMat(Shader *shader, char *name, glm::mat4 val);

		// if this is an instance of another object
		// we need to process it differently when rendering.
		bool isInstance;
		// note if the object info is being relied on by others.
		// if this one is cleared from memory, vertex buffers 
		// need to be shifted to somewhere else
		bool hasCopies;
		MeshObj *instanceOf;


		glm::mat4 modelMatrix; // movement, scale, and rotation (around pivot)
		glm::mat4 transformMatrix; // rotation info only - for normals

		glm::mat4 updateModelMatrix();
		glm::mat4 updateModelMatrix(glm::mat4 transform);

		void setVisible(bool state);
		bool getVisible();

		void setCullMode(char state);
		char getCullMode();

		void setDepthWrite(bool state);
		bool getDepthWrite();

		void setTransparency(bool state);
		bool getTransparency();

		void setShader(int shaderInd);
		int getShader();

		void setDepthRead(char mode);
		char getDepthRead();

		void calcMinMax();
		float getMinX();
		float getMaxX();
		float getMinY();
		float getMaxY();
		float getMinZ();
		float getMaxZ();

		// we only want to recalculate model matrices once
		// if pos/scale/rotation has been changed within 
		// the frame
		bool matrixUpdateRequired;

		void setPosition(float x, float y, float z);
		void setPosition(glm::vec3 p);
		glm::vec3 getPosition();

		void setRotation(float x, float y, float z);
		void setRotation(glm::vec3 p);
		glm::vec3 getRotation();

		void setScale(float x, float y, float z);
		void setScale(glm::vec3 p);
		glm::vec3 getScale();

		void setPivot(float x, float y, float z);
		void setPivot(glm::vec3 p);
		glm::vec3 getPivot();

		// add shader constant by individual uniforms
		void setShaderConst(Shader* shader, char *name, float v0, float v1, float v2, float v3);

		// call this when rendering to set the shader
		// with the correct uniform values
		void updateShaderUniforms(Shader *shader);

		// default facing assumption (0,0,1)
		glm::mat4 lookat(glm::vec3 lookatpos, float z_rotation);

		void getTangents();

		void clear();
	};

	struct CollisionData
	{
		CollisionData()
		{
			raylength = 0.0f;
			pos = glm::vec3(0);
			norm = glm::vec3(0);
			uv = glm::vec2(0);
			meshInd = -1;
		}
		CollisionData( const CollisionData &other)
		{
			raylength = other.raylength;
			pos = other.pos;
			norm = other.norm;
			uv = other.uv;
			meshInd = other.meshInd;
		}
		CollisionData(float len, glm::vec3 p, glm::vec3 n)
		{
			raylength = len;
			pos = p;
			norm = n;
			meshInd = -1;
		}

		float raylength;
		glm::vec3 pos;
		glm::vec3 norm;
		glm::vec2 uv;
		short int meshInd;
	};

	struct MeshManager
	{
		MeshManager()
		{
			meshArray = new MeshObj[256];
			arraySize = 256;
			meshCount = 0;
			
			defaultShader = NULL;
			defaultTexture = 0;

			raycastDataArray = new CollisionData[64];
			int raycastHitCount = 0;
			int raycastArraySize = 16;
			
			spherecastDataArray = new CollisionData[16];
			int spherecastHitCount = 0;
			int spherecastArraySize = 16;

			indexTree = Octree<VertexData>(6);

			static const GLfloat g_quad_vertex_buffer_data[] = {
				-1.0f, -1.0f, 0.0f,
				1.0f, -1.0f, 0.0f,
				-1.0f,  1.0f, 0.0f,
				-1.0f,  1.0f, 0.0f,
				1.0f, -1.0f, 0.0f,
				1.0f,  1.0f, 0.0f,
			};

			GLuint quad_VertexArrayID;
			glGenVertexArrays(1, &quad_VertexArrayID);
			glBindVertexArray(quad_VertexArrayID);

			glGenBuffers(1, &quad_vertexbuffer);
			glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

			lastGivenID = -1;
		}

		MeshManager(const MeshManager &other)
		{
			meshArray = other.meshArray;
			meshCount = other.meshCount;
			arraySize = other.arraySize;

			defaultShader = other.defaultShader;
			defaultTexture = other.defaultTexture;

			raycastDataArray = other.raycastDataArray;
			int raycastHitCount = other.raycastHitCount;
			int raycastArraySize = other.raycastArraySize;

		}
		int lastGivenID;
		int assignID(int index);
		int assignID();
		int getID(int index);

		GLuint quad_vertexbuffer;

		MeshObj *meshArray;
		unsigned short int arraySize;
		unsigned short int meshCount;

		int add(MeshObj mesh);

		int loadMesh(char *name);
		int createPlane(glm::vec2 size);
		int createLine3(glm::vec3 p0, glm::vec3 p1, float weight);
		int createQuad();

		void setTexture(int index, int texID, int texIndex);
		void setPosition(int index, glm::vec3 pos);
		void setRotation(int index, glm::vec3 rot);
		void setScale(int index, glm::vec3 scale);
		void setCollision(int index, bool state);
		void setVisible(int index, bool state);
		void setCullMode(int index, char state); // 0 = none, 1 = backface, 2 = frontface
		void setCastShadow(int index, bool state);
		void setIsWater(int index, bool state);
		void setReflectable(int index, bool state);
		void setFollowCam(int index, bool state);
		void setTransparent(int index, bool state);
		void setLookatCam(int index, bool state);

		int getTexture(int index, int texIndex);
		glm::vec3 getPosition(int index);
		glm::vec3 getRotation(int index);
		glm::vec3 getScale(int index);
		bool getCollision(int index);
		bool getVisible(int index);
		char getCullMode(int index); // 0 = none, 1 = backface, 2 = frontface
		bool getCastShadow(int index);
		bool getIsWater(int index);
		bool getReflectable(int index);
		bool getFollowCam(int index);
		bool getTransparent(int index);
		bool getLookatCam(int index);

		glm::mat4 MeshManager::lookat(glm::vec3 pos, glm::vec3 lookatpos, float z_rotation);
		void MeshManager::lookat(int index, glm::vec3 pos, glm::vec3 lookatpos, float z_rotation);

		int copy(int index);
		void deleteMesh(int index);
		int attach(int index0, int index1);
		void exportMesh(int index);

		// raycasting
		bool raycastMesh(int id, glm::vec3 p0, glm::vec3 p1);
		// raycast all mesh except the ones that have collision turned off
		bool raycastAllMesh( glm::vec3 p0, glm::vec3 p1);
		// raycast all mesh even if collision is turned off
		bool raycastAllMeshIgnoreStates(glm::vec3 p0, glm::vec3 p1); 
		bool spherecastMesh(int id, glm::vec3 center, float radius);
		bool spherecastAllMesh( glm::vec3 center, float radius);
		bool raycastTriangle( glm::vec3 p0, glm::vec3 p1, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);
		bool spherecastTriangle( glm::vec3 center, float radius, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);
		glm::vec3 resolveSphereSlide();
		
		//glm::vec2 PosToUV(int id, glm::vec3 pos);
		glm::vec3 interpolateTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 p);
		glm::vec4 textureValueFromUV(int id, glm::vec2 uv, int texid);


		int raycast_index[3];
		int indexCount;
		glm::vec3 raycast_vert[3];
		glm::vec3 raycast_norm[3];

		glm::vec3 raydir;
		float raylength;
		float dotNormRaydir;
		glm::vec3 sampleSphereCenter;
		float sampleSphereRadius;

		// ray cast data & management
		CollisionData *raycastDataArray;
		int raycastHitCount;
		int raycastArraySize;

		// sphere cast data & management
		CollisionData *spherecastDataArray;
		int spherecastHitCount;
		int spherecastArraySize;

		CollisionData *sortingArray;
		unsigned short sortingHitCount;

		// sort raycast data
		void sortRayCastData();
		void sortSphereCastData();
		void recQuickSort(int left, int right);
		int partitionIt(int left, int right, CollisionData pivot);
		void swap(int dex1, int dex2);

		// multithread items
		volatile MeshObj *threadLoadMesh;


		//shaders
		ShaderManager * shaderManager;
		void setShader(unsigned int meshIndex, unsigned int shaderIndex);

		void setShaderConst(int meshIndex, char *name, float v0, float v1, float v2, float v3);
		void setShaderConst(int meshIndex, char *name, int v0, int v1, int v2, int v3);
		
		void setShaderMat(int meshIndex, char *name, glm::mat4 val);

		// shaders need to be index values. Not pointers
		unsigned short int defaultShader;
		unsigned short int lineShader;
		GLuint defaultTexture;
		GLuint defaultNormalTex;

		TextureManager *textureManager;

		void frustumCull(Camera cam, glm::mat4 proj, glm::vec2 res);


		Octree<VertexData> indexTree;

		void indexVBO(
			MeshFileData in_data,

			Stack<unsigned int> & out_indices,
			Stack<glm::vec3> & out_vertices,
			Stack<glm::vec2> & out_uvs,
			Stack<glm::vec3> & out_normals,
			Stack<glm::vec3> & out_colors
		);
	};



}

#endif
