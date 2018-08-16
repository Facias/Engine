#ifndef ENGINE_MAIN_H_
#define ENGINE_MAIN_H_

#include <ctime>
#include <chrono>
#include "glm.hpp"
#include <string>
#include <assert.h>
#include <mutex>

#include <glew.h>
#include <glfw3.h>
#include <wglew.h>

//#include <nvtt.h>

#include "controls.h"
#include "Text2D.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Particle.h"
#include "Camera.h"
#include "SceneScript.h"
#include "Sprite.h"
#include "Network.h"

#include "Octree.h"
#include "Editor.h"


#define WIDTH 1920
#define HEIGHT 1080

#define completePastWritesBeforeFutureWrites _WriteBarrier(); _mm_sfence()
#define completePastReadsBeforeFutureReads _ReadBarrier()

/*
 InterlockedIncrement()
*/

/* suggested fixed texture locations
	MS refractable buffer
	reflection
	MS depth
	depth
	MS opaque
	shadows

*/

struct ImageData
{
	char* name;
	unsigned short int width;
	unsigned short int height;
	unsigned char depth;

	glm::vec4 *data;

};

struct GISamplePoint
{

	GISamplePoint()
	{

	}
	glm::vec3 pos;
	glm::vec2 uv;
	glm::vec3 norm;

	glm::vec4 lightVal;
};



namespace ENGINE
{
	struct SortItem2D;


	struct win32_thread_info
	{
		int logicalThreadIndex;
		HANDLE SemaphoreHandle;
	};

	struct work_queue_entry
	{
		char *stringToPrint;
	};

	static unsigned int volatile nextEntryToDo;
	static unsigned int volatile entryCount;
	static unsigned int volatile entryCompletionCount;
	static short int threadCount;
	static work_queue_entry entries[256];

	static void PushString(char *str)
	{

		work_queue_entry *Entry = entries + entryCount;
		Entry->stringToPrint = str;

		completePastWritesBeforeFutureWrites;

		++entryCount;
	}

	//HANDLE SemaphoreHandle = CreateSemaphor(0, 0, threadCount, 0, 0, SEMAPHORE_ALL_ACCESS);

	struct Thread
	{
		DWORD ID;
		HANDLE handle;

		DWORD WINAPI proc(LPVOID lpParameter)
		{
			win32_thread_info *threadInfo = (win32_thread_info *)lpParameter;

			for (;;)
			{
				if (nextEntryToDo < entryCount)
				{
					int entryIndex = InterlockedIncrement((LONG volatile *)&nextEntryToDo) - 1;

					completePastReadsBeforeFutureReads;

				}
				else
				{
					WaitForSingleObjectEx(threadInfo->SemaphoreHandle, INFINITE, FALSE);
				}
			}

		}
	};

	struct Scene
	{
		Stack<MeshObj> meshset;
	};

	struct Engine
	{
		void init();
		void mainLoop(void);

		void render(void);


		Editor editor;
		Thread *workingThread;

		double time;
		std::chrono::high_resolution_clock::time_point last_clock;
		std::chrono::high_resolution_clock::time_point clock;
		std::chrono::duration<double> duration;
		double frameTime;

		double getFrameTime();
		double getFrameRate();
		void updateFrameTimeInfo();

		Network network;
		TextureManager textureManager;

		void setVSync(char state);
		bool VSyncState;

		char samples;
		void setMultisampleLevel(char lv);
		void setResolution(int width, int height);

		
		

		//string methods
		char* strToChar(std::string str);
		char* strToken(char* str, char* delimiter, int token);

		

		glm::ivec2 getWindowSize();
		void setWindowSize(int width, int height);

		// scene script
		SceneScript sceneScript;
		void buildSceneEntity(int index);
	
		SpriteManager spriteManager;

		

		//mesh
		MeshManager meshManager;
		// load mesh and return index to meshArray where it exists
		//int loadMesh(char *name);
		int instanceMesh(int index);
		//MeshObj *current; // current mesh to be rendered
		//MeshObj *referenceMesh;
		glm::mat4 *currentModel;

		// RenderQuad
		int renderQuad;

		void drawMesh(MeshObj *current);
		void drawMeshGI(MeshObj *current);
		void drawMeshShadowMap(MeshObj *current, int cascade);
		//void drawQuad(QuadObj *current);
		void drawParticle(ParticleSet *current);
		void drawSprite(Sprite *current);
		void drawSelectedMesh(MeshObj *current);
		void drawText(int index);

		ShaderManager shaderManager;
		void recompileShaders();
		bool updateShaderState;


		RenderTexture renderImgMS[2];
		RenderTexture renderImg[2];
		RenderTexture depthBuffer;
		
		RenderTexture reflectionImg;

		RenderTexture lightShaftBuffer[3];
		int lightShaftQuad;
		int lightShaftShader;

		// shadows
		RenderTexture shadowMap[4];
		RenderTexture shadowMapComp;
		glm::mat4 shadowOrthoProj[4];
		glm::mat4 shadowCamView[4];
		glm::mat4 shadowViewProj[4];
		glm::mat4 mvp_shadow;
		glm::vec3 cascadeCenter[4];
		float shadowmapRes;
		float cascadeRadius[4]; // radius of cascade sphere
		float cascadeCenterDist[4]; // distance from camera to cascade center
		short int shadowCompShader;
		int shadowCompQuad;

		//GI shadows
		RenderTexture GI_shadowMap[4];
		RenderTexture GI_shadowMapComp;
		float GI_shadowmapRes;
		int GI_shadowCompQuad;
		

		//global illumination
		RenderTexture lightSampleGIBuffer[5];
		void SampleVertexGI(MeshObj *mesh, bool doublesided);
		int createLightMap(MeshObj *mesh, float size);
		Camera GICamera;
		bool pointInTriangleUV(glm::vec2 uv0, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 point);
		glm::vec3 interpolateTriangle(glm::vec3 vert0, glm::vec3 vert1, glm::vec3 vert2, glm::vec3 point);
		glm::vec3 interpolateTriangleUV(glm::vec2 uv0, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 point);
		bool moe(float input, float check, float margin);
		Stack<GISamplePoint> gisamplepoint;
		void samplepoint(glm::vec3 pos, glm::vec3 norm, float rotation, bool foliage);

		glm::mat4 newView[4];
		glm::mat4 newViewProj[4];

		static float area_element(float x, float y);
		static float solid_angle_term(const glm::vec2 & f, int cube_size);
		static float clamped_cosine_term(const glm::vec3 & dir);

		// bloom
		RenderTexture bloomBuffer[2];
		short int bloomShader;
		int bloomQuad;

		// blur
		RenderTexture convolutionBuffer[1];
		short int convolutionShader;
		void blurBuffer(GLuint input, RenderTexture output, int kernel, float spread);
		int blurQuad;
		void extractHighlights(GLuint input);

		// aperture
		RenderTexture lightSampleImg;
		int lightSampleQuad;
		short int lightSampleShader;
		void sampleLight(RenderTexture input);
		int apertureAdjustTimer;

		// FXAA
		RenderTexture FXAA_lumaBuffer;
		short int FXAA_lumaShader;
		short int FXAA_Shader;
		int FXAA_Quad;
		void antialiasBuffer(RenderTexture input);


		Camera sunCamera;
		
		glm::mat4 look_right, look_left, look_up, look_down;

		void setRenderToTexture(RenderTexture rt, GLint depthTex);
		void setRenderToScreen();
		void resolveMultisample(RenderTexture MSbuffer, RenderTexture Nbuffer);
		void resolveMultisampleDepth(RenderTexture MSbuffer, RenderTexture Nbuffer);
		void copyRenderBuffer(RenderTexture buffer0, RenderTexture buffer1);

		glm::ivec2 windowSize;
		

		glm::vec3 screenPointToVector(glm::vec2 point);
		glm::vec2 pointToScreenPos(glm::vec3 point);
		static glm::vec3 screenPointToVector(glm::vec2 point, Camera cam);
		static glm::vec2 pointToScreenPos(glm::vec3 point, Camera cam, glm::vec2 resolution);


		//render 3d
		glm::mat4 mvp;
		float reflectionHeight = .9f;

		Controls control;
		GLFWwindow *window;

		ParticleSet particles;
		ParticleManager particleManager;

		//GLuint programID;
		GLuint Texture;
		//GLuint vertexbuffer, uvbuffer, normalbuffer;
		GLuint MatrixID;
		GLuint TransformMatrixID;
		GLuint modelMatrixID;


		
		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix;
		glm::mat4 view_projMatrix;

		static glm::mat4 lookat(glm::vec3 pos, glm::vec3 lookatpos, float z_rotation);
		static glm::mat4 translate(glm::vec3 pos);
		static glm::mat4 scale(glm::vec3 s);

		TextManager textManager;

		// default shaders must be indices, not pointers
		short int defaultShader;
		short int defaultQuadShader;
		short int defaultParticleShader;
		short int defaultFoliageShader;
		short int defaultWaterShader;
		short int defaultShadowShader;
		short int defaultSpriteShader;
		short int editorSelectShader;
		GLuint defaultTexture;
		
		Camera mainCamera;
		Camera reflectCamera;

		bool drawWireFrame;

		PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

		glm::vec3 ShadowCascadeCenter;
		
		void genMipmaps(TextureObj tex);

		

		// track gpu states 
		// always check and update these states 
		// before and after changing shaders and textures
		Shader * shaderState;
		GLint textureStateID[16];
		Shader *hold; // rendertime shader swapping
		glm::vec2 res;

	

		static glm::vec3 raycastPlane(glm::vec3 p0, glm::vec3 p1, glm::vec3 norm, glm::vec3 orig);

		CameraManager cameraManger;

		void sort2DItems();
		void recQuickSort(int left, int right);
		int partitionIt(int left, int right, SortItem2D pivot);
		void swapItem2D(int dex1, int dex2);

		SortItem2D *sorting2DArray;
		int sorting2DCount;
		//float aspectRatio;

		void frustumCull(Camera cam);
		

		char processorCount;
	};

	

	struct SortItem2D
	{
		SortItem2D()
		{
			arrayIndex = -1;
			depth = 10.0;
			type = SPRITE;
		}

		SortItem2D(int _index, float _depth, unsigned char _type)
		{
			type = _type;
			depth = _depth;
			arrayIndex = _index;
		}

		SortItem2D(const SortItem2D &other)
		{
			type = other.type;
			depth = other.depth;
			arrayIndex = other.arrayIndex;
		}
			
		enum
		{
			SPRITE,
			TEXT
		};

		unsigned char type;
		int arrayIndex;
		float depth;
	};
}

#endif
