#include "EngineMain.h"
#include <Windows.h>

#include "gtc/matrix_transform.hpp"
#include "gtx/transform2.hpp"
#include "boost/filesystem.hpp"

#include "wglext.h"
#include <iostream>
#include <chrono>
#include <fstream>


#include "Texture.h"
#include "Text.h"
#include "Shader.h"



#undef max
#undef min

using namespace ENGINE;



void Engine::init()
{
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	setVSync(0);

	sorting2DArray = new SortItem2D[8];

	sunpos = glm::vec3(-100000.f, -20000.f, 0.f);
	sundir = glm::normalize(sunpos);


	/*
	std::ofstream file("Shoob.txt");
	file << "lumpy" << std::endl;

	file.close();
	
	std::ifstream fole("shoob.txt");
	std::string str;
	std::getline(fole, str);
	
	char *temp = strToChar(str);
	
	float lala = strToFloat("15.65");
	std::cout << lala - 7.0 << std::endl;

	char *shoob = strToken("1 2, 3 4", " ,", 2);
	std::cout << shoob << std::endl;

	OutputDebugStringA(temp);
	OutputDebugStringA("\n");
	*/

	//network.getIP("www.google.com");
	//network.bindSocket();

	for (int n = 0; n < 16; n++)
		textureStateID[n] = -2;


	meshManager.shaderManager = &shaderManager; //do this before assigning any shaders to mesh
	meshManager.textureManager = &textureManager;
	spriteManager.shaderManager = &shaderManager;
	spriteManager.textureManager = &textureManager;
	textManager.textureManager = &textureManager;
	textManager.shaderManager = &shaderManager;

	defaultParticleShader = shaderManager.loadShader("particleVertex.vs", "particleFrag.ps");
	defaultShader = shaderManager.loadShader("vertex.vs", "frag.ps");
	defaultFoliageShader = shaderManager.loadShader("vertex.vs", "foliageFrag.ps");
	defaultQuadShader = shaderManager.loadShader("quad.vs", "finalFrag.ps");
	defaultWaterShader = shaderManager.loadShader("vertex.vs", "waterFrag.ps");
	defaultShadowShader = shaderManager.loadShader("vertex.vs", "shadowFrag.ps");
	defaultSpriteShader = shaderManager.loadShader("spriteVertex.vs", "spriteShader.ps");

	shadowCompShader = shaderManager.loadShader("quad.vs", "shadowCompShader.ps");
	convolutionShader = shaderManager.loadShader("quad.vs", "blur.ps");
	bloomShader = shaderManager.loadShader("quad.vs", "bloomExtend.ps");

	FXAA_lumaShader = shaderManager.loadShader("quad.vs", "storeLuma.ps");
	FXAA_Shader = shaderManager.loadShader("quad.vs", "FXAA.ps");
	int lightShader = shaderManager.loadShader("vertex.vs", "lightFrag.ps");

	lightSampleShader = shaderManager.loadShader("quad.vs", "lightSampleShader.ps");
	int skyShader = shaderManager.loadShader("vertex.vs", "skyShader.ps");
	editorSelectShader = shaderManager.loadShader("highlightVertex.vs", "editorSelectMeshShader.ps");
	int lineShader = shaderManager.loadShader("vertex.vs", "lineFrag.ps");

	lightShaftShader = shaderManager.loadShader("quad.vs", "lightShaft.ps");

	meshManager.lineShader = lineShader;
	meshManager.defaultShader = defaultShader;

	defaultTexture = textureManager.loadImage("white.png");
	meshManager.defaultTexture = defaultTexture;
	int defaultNormalTex = textureManager.loadImage("normalBlank.png");
	meshManager.defaultNormalTex = defaultNormalTex;


	spriteManager.defaultSpriteShaderID = defaultSpriteShader;
	
	

	res = glm::vec2(WIDTH, HEIGHT);
	spriteManager.res = res;

	renderImg[0] = textureManager.createRenderTexture(res.x, res.y, false);
	renderImg[1] = textureManager.createRenderTexture(res.x, res.y, false);

	// global illumination
	int directShader = shaderManager.loadShader("vertex.vs", "directShader.ps");
	lightSampleGIBuffer[0] = textureManager.createRenderTexture(32, 32, true); // 0,0,1
	lightSampleGIBuffer[1] = textureManager.createRenderTexture(32, 32, true); // 1,0,0
	lightSampleGIBuffer[2] = textureManager.createRenderTexture(32, 32, true); // -1,0,0
	lightSampleGIBuffer[3] = textureManager.createRenderTexture(32, 32, true); // 0,1,0
	lightSampleGIBuffer[4] = textureManager.createRenderTexture(32, 32, true); // 0,-1,0
	//int testplane = meshManager.createPlane(glm::vec2(10));
	//meshManager.meshArray[testplane].texture[0] = lightSampleGIBuffer[0].color;
	//meshManager.setShaderConst(testplane, "vcolor", 1, 1, 1, 1);
	//meshManager.setShaderConst(testplane, "tile", 1, 1, 0, 0);
	//meshManager.setTexture(testplane, defaultTexture, 0);
	//meshManager.setShader(testplane, directShader);

	// simple rotations
	look_right = { 0,0,-1,0,
				   0,1,0,0,
				   1,0,0,0,
				   0,0,0,1 };

	look_left = { 0,0,1,0,
				  0,1,0,0,
				  -1,0,0,0,
				  0,0,0,1 };

	look_up = { 1,0,0,0,
				0,0,-1,0,
				0,1,0,0,
				0,0,0,1 };

	look_down = { 1,0,0,0,
				  0,0,1,0,
				  0,-1,0,0,
				  0,0,0,1 };

	reflectionImg = textureManager.createRenderTexture(res.x*1.f, res.y*1.f, false);

	// shadows
	shadowmapRes = 1024;
	shadowMap[0] = textureManager.createRenderTexture(shadowmapRes, shadowmapRes, false);
	shadowMap[1] = textureManager.createRenderTexture(shadowmapRes, shadowmapRes, false);
	shadowMap[2] = textureManager.createRenderTexture(shadowmapRes, shadowmapRes, false);
	shadowMap[3] = textureManager.createRenderTexture(shadowmapRes, shadowmapRes, false);
	shadowMapComp = textureManager.createRenderTexture(shadowmapRes*4, shadowmapRes, false);


	glBindTexture(GL_TEXTURE_2D, shadowMapComp.colorID);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, GLint(0));

	glBindTexture(GL_TEXTURE_2D, shadowMapComp.depthID);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, GLint(0));
	

	// GI shadows
	GI_shadowmapRes = 256;
	GI_shadowMap[0] = textureManager.createRenderTexture(shadowmapRes, shadowmapRes, false);
	GI_shadowMap[1] = textureManager.createRenderTexture(shadowmapRes, shadowmapRes, false);
	GI_shadowMap[2] = textureManager.createRenderTexture(shadowmapRes, shadowmapRes, false);
	GI_shadowMap[3] = textureManager.createRenderTexture(shadowmapRes, shadowmapRes, false);
	GI_shadowMapComp = textureManager.createRenderTexture(shadowmapRes * 4, shadowmapRes, false);


	glBindTexture(GL_TEXTURE_2D, shadowMapComp.colorID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	glBindTexture(GL_TEXTURE_2D, shadowMapComp.depthID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);


	sceneScript.parseFile("scene.ss");
	
	int vv;
	for (uint i = 0; i < 5; i++)
	{
		vv = meshManager.createPlane(glm::vec2(10));
		meshManager.setTexture(vv, lightSampleGIBuffer[i].color, 0);
		meshManager.setTexture(vv, defaultNormalTex, 2);
		meshManager.setPosition(vv, glm::vec3(i * 10, 20, 10));
		meshManager.meshArray[vv].shaderIndex = defaultShader;
		meshManager.setShaderConst(vv, "samples", 8, 0, 0, 0);
		meshManager.setShaderConst(vv, "resolution", res.x, res.y, 0, 0);
		meshManager.setShaderConst(vv, "tile", 0, 0, 0, 0);
		meshManager.setShaderConst(vv, "vcolor", 1.0f, 1.0f, 1.0f, 1.0f);
		meshManager.setShaderConst(vv, "GI", 0, 0, 0, 0);
		meshManager.setTexture(vv, shadowMapComp.depth, 15);
	}

	textManager.init();
	textManager.loadFont("CambriaFont72.png");

	// aperture
	lightSampleQuad = meshManager.createQuad();
	lightSampleImg = textureManager.createRenderTexture(256, 256, true);
	meshManager.setShader(lightSampleQuad, lightSampleShader);

	// blur
	blurQuad = meshManager.createQuad();
	convolutionBuffer[0] = textureManager.createRenderTexture(res.x, res.y, false);
	meshManager.setShader(blurQuad, convolutionShader);
	
	// bloom
	bloomQuad = meshManager.createQuad();
	bloomBuffer[0] = textureManager.createRenderTexture(res.x / 2, res.y / 2, false);
	bloomBuffer[1] = textureManager.createRenderTexture(res.x / 4, res.y / 4, false);
	meshManager.setShader(bloomQuad, bloomShader);
	meshManager.setTexture(bloomQuad, lightSampleImg.color, 2);
	

	// FXAA
	FXAA_Quad = meshManager.createQuad();
	FXAA_lumaBuffer = textureManager.createRenderTexture(res.x, res.y, false);

	// lightshafts
	lightShaftBuffer[0] = textureManager.createRenderTexture(res.x, res.y, false);
	lightShaftBuffer[1] = textureManager.createRenderTexture(res.x, res.y, false);
	lightShaftBuffer[2] = textureManager.createRenderTexture(res.x/2, res.y/2, false);

	lightShaftQuad = meshManager.createQuad();
	meshManager.setTexture(lightShaftQuad, renderImg[0].depth, 0);
	meshManager.setShader(lightShaftQuad, lightShaftShader);

	samples = 0;
	
	//shaderManager.setMultisampleState(samples);
	if (samples > 0)
	{
		renderImgMS[0] = textureManager.createRenderTextureMultisample(res.x, res.y, samples);
	}


	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_LESS, .01);

	// compiled shadow map
	shadowCompQuad = meshManager.createQuad();
	meshManager.setTexture(shadowCompQuad, shadowMap[0].depth, 0);
	meshManager.setTexture(shadowCompQuad, shadowMap[1].depth, 1);
	meshManager.setTexture(shadowCompQuad, shadowMap[2].depth, 2);
	meshManager.setTexture(shadowCompQuad, shadowMap[3].depth, 3);
	meshManager.setTexture(shadowCompQuad, shadowMap[0].color, 4);
	meshManager.setTexture(shadowCompQuad, shadowMap[1].color, 5);
	meshManager.setTexture(shadowCompQuad, shadowMap[2].color, 6);
	meshManager.setTexture(shadowCompQuad, shadowMap[3].color, 7);
	meshManager.setShader(shadowCompQuad, shadowCompShader);

	// GI compiled shadow map
	GI_shadowCompQuad = meshManager.createQuad();
	meshManager.setTexture(GI_shadowCompQuad, GI_shadowMap[0].depth, 0);
	meshManager.setTexture(GI_shadowCompQuad, GI_shadowMap[1].depth, 1);
	meshManager.setTexture(GI_shadowCompQuad, GI_shadowMap[2].depth, 2);
	meshManager.setTexture(GI_shadowCompQuad, GI_shadowMap[3].depth, 3);
	meshManager.setTexture(GI_shadowCompQuad, GI_shadowMap[0].color, 4);
	meshManager.setTexture(GI_shadowCompQuad, GI_shadowMap[1].color, 5);
	meshManager.setTexture(GI_shadowCompQuad, GI_shadowMap[2].color, 6);
	meshManager.setTexture(GI_shadowCompQuad, GI_shadowMap[3].color, 7);
	meshManager.setShader(GI_shadowCompQuad, shadowCompShader);

	for (int n = 0; n < sceneScript.entityCount; n++)
		buildSceneEntity(n);
	
	particles = ParticleSet(10);
	particles.shaderIndex = defaultParticleShader;

	for (uint n = 0; n < particles.count; n++)
		particles.pos[n] = glm::vec3((rand()%2000)*.1f-100, 0, (rand()%2000)*.1f-100);

	glBindBuffer(GL_ARRAY_BUFFER, particles.posBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*particles.count, particles.pos, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, particles.rotBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*particles.count, particles.rot, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, particles.scaleBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*particles.count, particles.scale, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, particles.colorBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*particles.count, particles.color, GL_STREAM_DRAW);

	int grass = textureManager.loadImage("sparkle.DDS");

	
	particles.texture[0] = textureManager.texArray[grass].id;

	frameTime = 0.0;
	time = 0.0;
	clock = std::chrono::high_resolution_clock::now();

	for (int n = 0; n < meshManager.meshCount; n++)
		meshManager.meshArray[n].updateModelMatrix();

	control.Init(window);

	
	renderQuad = meshManager.createQuad();
	meshManager.setTexture(renderQuad, renderImg[0].color, 0);
	meshManager.setTexture(renderQuad, bloomBuffer[1].color, 1);
	meshManager.setTexture(renderQuad, lightSampleImg.color, 2);
	meshManager.setTexture(renderQuad, renderImg[0].depth, 10);
	meshManager.setShader(renderQuad, defaultQuadShader);
	meshManager.setTexture(renderQuad, lightShaftBuffer[1].color, 3);

	
	//GLuint quad_VertexArrayID;
	//glGenVertexArrays(1, &quad_VertexArrayID);
	//glBindVertexArray(quad_VertexArrayID);

	

	control.position = glm::vec3(0, 4, -4);


	GLint units;
	glGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS, &units);



	control.window = window;
	control.cameraMode = Controls::CAM_FLY;
	drawWireFrame = false;

	
	// setup editor features
	editor.textManager = &textManager;
	editor.spriteManager = &spriteManager;
	editor.meshManager = &meshManager;
	editor.shaderManager = &shaderManager;
	editor.textureManager = &textureManager;
	editor.shadowmapcomp_depth = shadowMapComp.depth;
	editor.windowSize = windowSize;
	editor.mainCamera = &mainCamera;
	editor.control = &control;
	editor.Init();

	

	shaderState = &Shader();
	apertureAdjustTimer = 0;

	//printf("\n\n\n\n");
	//printf( boost::filesystem::initial_path().string().c_str() );
	
	
	control.typeInText = textManager.createText("");
	textManager.setPosition(control.typeInText, glm::vec2(0, 40));
	textManager.setVisible(control.typeInText, false);

	setMultisampleLevel(8);


	//meshManager.createLine3(glm::vec3(0), glm::vec3(500,200,000), .1f);
	//meshManager.createLine3(glm::vec3(0,0,10), glm::vec3(500, 200, 000), .1f);

	//projectionMatrix = glm::perspective(glm::radians(control.activeCamera.FOV), 16.0f / 9.0f, 0.1f, 1000.0f);
	mainCamera.projMatrix = glm::perspective(glm::radians(control.activeCamera.FOV), 16.0f / 9.0f, 0.1f, 2500.0f);




	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	processorCount = (char)sysinfo.dwNumberOfProcessors;

	workingThread = new Thread[processorCount];
	//setResolution(640, 480);

	
	return;
}

void Engine::mainLoop(void)
{
	editor.editorMode = control.editorMode;
	meshManager.setShaderConst(renderQuad,"time", time, 0.f, 0.f, 0.f);
	
	

	if (control.position.y > 19.882)
	{
		reflectionHeight = 19.882f;
	}
	else
	{
		reflectionHeight = 0.f;
	}


	//printf("%f\n", duration);
//	glm::mat4 tmat = CameraManager::lookatMatrix(mainCamera, mainCamera.pos + mainCamera.forward, 0.0f);
//	glm::mat4 upmat = CameraManager::lookatMatrix(mainCamera, mainCamera.pos + glm::vec3(tmat[0]), 0.0f);
//	printf("tmat %f %f %f ->", tmat[0].x, tmat[0].y, tmat[0].z);
//	printf("upmat %f %f %f \n", upmat[0].x, upmat[0].y, upmat[0].z);
	//printf("up %f %f %f %f\n", tmat[2].x, tmat[2].y, tmat[2].z, tmat[2].w);
	//printf("%f %f %f\n", mainCamera.forward.x, mainCamera.forward.y, mainCamera.forward.z);
	//printf("%f %f %f\n", mainCamera.right.x, mainCamera.right.y, mainCamera.right.z);

	if (editor.editorMode)
	{
		editor.windowSize = windowSize = control.windowSize;
		double w = static_cast<double> (windowSize.x);
		double h = static_cast<double> (windowSize.y);
		double ar = w / h;

		mainCamera.projMatrix = glm::perspective( glm::radians(control.activeCamera.FOV), 16.0f/9.0f, 0.1f, 2500.0f);
	}

	updateFrameTimeInfo();
	time += float(60.0 / glm::max(1.0, getFrameRate()));	

	
	control.frameRate = getFrameRate();
	control.update();

	// switch multisample level
	control.updateKeystate(GLFW_KEY_1);
	KeyState state = control.keystate[GLFW_KEY_1];
	if (state.down && state.downtimer == 0)
		setMultisampleLevel(0);

	control.updateKeystate(GLFW_KEY_2);
	state = control.keystate[GLFW_KEY_2];
	if (state.down && state.downtimer == 0)
		setMultisampleLevel(2);

	control.updateKeystate(GLFW_KEY_3);
	state = control.keystate[GLFW_KEY_3];
	if (state.down && state.downtimer == 0)
		setMultisampleLevel(4);

	control.updateKeystate(GLFW_KEY_4);
	state = control.keystate[GLFW_KEY_4];
	if (state.down && state.downtimer == 0)
		setMultisampleLevel(8);

	control.updateKeystate(GLFW_KEY_5);
	state = control.keystate[GLFW_KEY_5];
	if (state.down && state.downtimer == 0)
		setMultisampleLevel(16);

	if (editor.editorMode)
	{
		control.updateKeystate(GLFW_KEY_0);
		state = control.keystate[GLFW_KEY_0];
		if (state.down && state.downtimer == 0)
		{
			if (editor.selectCount == 1)
			{
				

				SampleVertexGI(&meshManager.meshArray[editor.selectedMesh[0]], false);
				meshManager.setShaderConst(editor.selectedMesh[0], "GI", 1, 1, 0, 0);
				meshManager.meshArray[editor.selectedMesh[0]].useVertexLight = true;
			}
		}

		control.updateKeystate(GLFW_KEY_9);
		state = control.keystate[GLFW_KEY_9];
		if (state.down && state.downtimer == 0)
		{
			if (editor.selectCount == 1)
			{
				//createLightMap(&meshManager.meshArray[editor.selectedMesh[0]], 256);
			}
		}
	}


	// toggle wireframe
	control.updateKeystate(GLFW_KEY_F3);
	state = control.keystate[GLFW_KEY_F3];
	if (drawWireFrame)
	{
		if (state.down && state.downtimer == 0)
			drawWireFrame = false;
	}
	else
	{
		if (state.down && state.downtimer == 0)
			drawWireFrame = true;
	}

	// refresh shaders
	control.updateKeystate(GLFW_KEY_F4);
	state = control.keystate[GLFW_KEY_F4];
	if (!updateShaderState && state.down && state.downtimer == 0)
	{
		recompileShaders();
		updateShaderState = true;
	}
	else
	{
		updateShaderState = false;
	}

	// toggle camera collision
	control.updateKeystate(GLFW_KEY_F2);
	state = control.keystate[GLFW_KEY_F2];
	switch (control.cameraMode)
	{
		case Controls::CAM_WALK:
			if (state.down && state.downtimer == 0)
				control.cameraMode = Controls::CAM_FLY;
			break;

		case Controls::CAM_FLY:
			if (state.down && state.downtimer == 0)
				control.cameraMode = Controls::CAM_WALK;
			break;
	}
	
	if (control.cameraMode == Controls::CAM_WALK)
	{
		

		for (int n = 0; n < 1; n++)
		{
			
			meshManager.raycastAllMesh(control.position + glm::dvec3(0, 2, 0), control.position + glm::dvec3(0, -20, 0));
			if (meshManager.raycastHitCount > 0)
			{
				meshManager.sortRayCastData();
				control.position.y = meshManager.raycastDataArray[0].pos.y +  3;
			}
		
			meshManager.spherecastAllMesh(control.position, 1.5);
			if (meshManager.spherecastHitCount > 0)
			{
				glm::vec3 p = meshManager.resolveSphereSlide();
				control.position.x = p.x;
				control.position.z = p.z;
			}

			meshManager.raycastAllMesh(control.position + glm::dvec3(0, 2, 0), control.position + glm::dvec3(0, -20, 0));
			if (meshManager.raycastHitCount > 0)
			{
				meshManager.sortRayCastData();
				control.position.y = meshManager.raycastDataArray[0].pos.y + 3;
			}
		}

	}


	mainCamera.pos = control.position;
	mainCamera.forward = control.direction;
	mainCamera.up = control.up;
	mainCamera.right = control.right;

	mainCamera.viewMatrix = CameraManager::lookatMatrix(mainCamera, mainCamera.pos + mainCamera.forward, 0);
	mainCamera.view_projMatrix = mainCamera.projMatrix * mainCamera.viewMatrix;
	reflectCamera = CameraManager::reflectCam(mainCamera, glm::vec3(0, 1, 0), reflectionHeight);
	//reflectCamera.updateLocalVectors();

	
	for (int n = 0; n < meshManager.meshCount; n++)
	{
		glm::mat4 uu;
		bool updated = false;
		if (meshManager.meshArray[n].lookatCam)
		{
			uu = meshManager.meshArray[n].lookat(mainCamera.pos, 0.0f);
			updated = true;
			
		}
		if (meshManager.meshArray[n].follow)
		{
			meshManager.meshArray[n].pos = mainCamera.pos + meshManager.meshArray[n].followPos;
			updated = true;
		}
		if (updated)
			meshManager.meshArray[n].updateModelMatrix(uu);

		
	}
	
	// preform editor tasks
	if (editor.editorMode)
	{
		editor.Proc();
	}
	else
	{
		editor.Cleanup();
	}
	
	control.lockCam = editor.lockCam;

	if (!control.mouse_leftDown)
	{
		editor.interactWindow = false;
		control.resetMouseInput();
	}

	/*
	GLboolean smooth;
	
	glGetBooleanv(GL_POLYGON_SMOOTH, &smooth);
	printf("%d \n", smooth);
	*/

	frustumCull(mainCamera);

	render();

}

void Engine::render(void)
{

	generateShadowMap();

	//render opaque geometry
	if (samples > 0)
	{
		setRenderToTexture(renderImgMS[0], 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);
		glEnable(GL_SAMPLE_ALPHA_TO_ONE); // this ensures proper coverage color 
	}
	else
	{
		setRenderToTexture(renderImg[0], 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	view_projMatrix = mainCamera.view_projMatrix; 
	
	
	// draw opaque geometry
	for (int n = 0; n < meshManager.meshCount; n++)
	{
		if (meshManager.meshArray[n].type == STANDARD)
		{
			if (!meshManager.meshArray[n].frustumCulled)
			{
				if (!meshManager.meshArray[n].isWater)
				{
					if (!meshManager.meshArray[n].transparent)
					{
						meshManager.setShaderConst(n, "campos", mainCamera.pos.x, mainCamera.pos.y, mainCamera.pos.z, 0.f);
						meshManager.setShaderConst(n, "time", (float)time, 0.f, 0.f, 0.f);
						meshManager.setShaderConst(n, "shadowMapSize", shadowmapRes, 0, 0, 0);
						meshManager.setShaderConst(n, "cascadeCenter0", cascadeCenter[0].x, cascadeCenter[0].y, cascadeCenter[0].z, 0.f);
						meshManager.setShaderConst(n, "cascadeCenter1", cascadeCenter[1].x, cascadeCenter[1].y, cascadeCenter[1].z, 0.f);
						meshManager.setShaderConst(n, "cascadeCenter2", cascadeCenter[2].x, cascadeCenter[2].y, cascadeCenter[2].z, 0.f);
						meshManager.setShaderConst(n, "cascadeCenter3", cascadeCenter[3].x, cascadeCenter[3].y, cascadeCenter[3].z, 0.f);
						drawMesh(&meshManager.meshArray[n]);
					}
				}
			}
		}
	}
	
	//if (samples > 0)
	//	glDisable(GL_SAMPLE_ALPHA_TO_ONE);
	
	// render reflection image
	
	if (mainCamera.pos.y > reflectionHeight)
	{
		setRenderToTexture(reflectionImg, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		reflectCamera.viewMatrix = CameraManager::lookatMatrix(reflectCamera, reflectCamera.pos + reflectCamera.forward, 3.14159);
		reflectCamera.projMatrix = mainCamera.projMatrix;

		view_projMatrix = reflectCamera.projMatrix * reflectCamera.viewMatrix;

		// reflect opaque 
		for (int n = 0; n < meshManager.meshCount; n++)
		{
			if (meshManager.meshArray[n].type == STANDARD)
			{
				if (meshManager.meshArray[n].reflectable)
				{
					if (!meshManager.meshArray[n].isWater)
					{
						if (!meshManager.meshArray[n].transparent)
						{

							glm::mat4 uu;
							bool updated = false;
							if (meshManager.meshArray[n].lookatCam)
							{
								uu = meshManager.meshArray[n].lookat(reflectCamera.pos, 0.0f);
								updated = true;

							}
							if (meshManager.meshArray[n].follow)
							{
								meshManager.meshArray[n].pos = reflectCamera.pos + meshManager.meshArray[n].followPos;
								updated = true;
							}
							if (updated)
								meshManager.meshArray[n].updateModelMatrix(uu);



							meshManager.setShaderConst(n, "reflectHeight", reflectionHeight, 0.f, 0.f, 0.f);
							drawMesh(&meshManager.meshArray[n]);
							meshManager.setShaderConst(n, "reflectHeight", -5000.f, 0.f, 0.f, 0.f);
						}
					}
				}
			}
		}

		// reflect transparent
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		if (samples > 0)
		{
			glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
			glDisable(GL_SAMPLE_ALPHA_TO_ONE);
		}
		for (int n = 0; n < meshManager.meshCount; n++)
		{
			if (!meshManager.meshArray[n].isWater)
			{
				if (meshManager.meshArray[n].transparent && meshManager.meshArray[n].reflectable)
				{
					meshManager.setShaderConst(n, "reflectHeight", reflectionHeight, 0.f, 0.f, 0.f);
					drawMesh(&meshManager.meshArray[n]);
					meshManager.setShaderConst(n, "reflectHeight", -5000.f, 0.f, 0.f, 0.f);
				}
			}
		}

		drawParticle(&particles);
		glDisable(GL_BLEND);

		// antialias reflections
		antialiasBuffer(reflectionImg);
	}// done reflecting

	view_projMatrix = mainCamera.view_projMatrix;

	if (samples > 1.0)
	{

		resolveMultisample(renderImgMS[0], renderImg[0]);

		setRenderToTexture(renderImgMS[0], renderImgMS[0].depthID);
	}
	else
	{
		copyRenderBuffer(renderImg[0], renderImg[1]);
		setRenderToTexture(renderImg[0], renderImg[0].depthID);
		
	}
	
	
	// render water
	glDepthMask(false);
	for (uint n = 0; n < meshManager.meshCount; n++)
	{
		if (meshManager.meshArray[n].isWater)
		{
			if (!meshManager.meshArray[n].frustumCulled)
			{
				meshManager.setShaderConst(n, "reflectHeight", reflectionHeight, 0.f, 0.f, 0.f);
				meshManager.setShaderConst(n, "time", (float)time, 0.f, 0.f, 0.f);
				meshManager.setShaderConst(n, "campos", mainCamera.pos.x, mainCamera.pos.y, mainCamera.pos.z, 0.f);
				meshManager.setShaderConst(n, "resolution", res.x, res.y, 0, 0);
				meshManager.setShaderConst(n, "shadowMapSize", shadowmapRes, 0, 0, 0);
				meshManager.setShaderConst(n, "cascadeCenter0", cascadeCenter[0].x, cascadeCenter[0].y, cascadeCenter[0].z, 0.f);
				meshManager.setShaderConst(n, "cascadeCenter1", cascadeCenter[1].x, cascadeCenter[1].y, cascadeCenter[1].z, 0.f);
				meshManager.setShaderConst(n, "cascadeCenter2", cascadeCenter[2].x, cascadeCenter[2].y, cascadeCenter[2].z, 0.f);
				meshManager.setShaderConst(n, "cascadeCenter3", cascadeCenter[3].x, cascadeCenter[3].y, cascadeCenter[3].z, 0.f);
				drawMesh(&meshManager.meshArray[n]);
			}
		}
	}
	glDepthMask(true);
	
	// draw alpha-blended geometry
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(false);
	for (int n = 0; n < meshManager.meshCount; n++)
	{
		if (!meshManager.meshArray[n].isWater)
		{
			if (meshManager.meshArray[n].transparent)
			{
				meshManager.setShaderConst(n, "campos", mainCamera.pos.x, mainCamera.pos.y, mainCamera.pos.z, 0.f);
				meshManager.setShaderConst(n, "cascadeCenter0", cascadeCenter[0].x, cascadeCenter[0].y, cascadeCenter[0].z, 0.f);
				meshManager.setShaderConst(n, "cascadeCenter1", cascadeCenter[1].x, cascadeCenter[1].y, cascadeCenter[1].z, 0.f);
				meshManager.setShaderConst(n, "cascadeCenter3", cascadeCenter[3].x, cascadeCenter[3].y, cascadeCenter[3].z, 0.f);
				meshManager.setShaderConst(n, "cascadeCenter2", cascadeCenter[2].x, cascadeCenter[2].y, cascadeCenter[2].z, 0.f);
				drawMesh(&meshManager.meshArray[n]);
			}
		}
	}

	drawParticle(&particles);
	
	
	glDepthMask(true);
	if (samples > 1)
	{
		//glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		//glEnable(GL_SAMPLE_ALPHA_TO_ONE);
	}
	glDisable(GL_BLEND);



	// outline selected mesh
	if (editor.editorMode)
	{

		for (int n = 0; n < editor.selectCount; n++)
		{
			if (editor.selectedMesh[n] >= 0)
				drawSelectedMesh(&meshManager.meshArray[editor.selectedMesh[n]]);
		}
		
	}
	
	if (drawWireFrame)
	{
		glDisable(GL_LINE_SMOOTH);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// if multisampling, resolve to to main buffer resolution
	if (samples > 0)
	{

		resolveMultisample(renderImgMS[0], renderImg[0]);
		//antialiasBuffer(renderImg[0]);
		//setRenderToTexture(renderImg[0], 0);
	}
	else
	{
		antialiasBuffer(renderImg[0]);
	}

	
	// prepass for light shafts
	
	setRenderToTexture(lightShaftBuffer[0], 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glm::vec2 sunpos2d = pointToScreenPos(-sunpos);
	float looksun = glm::dot(mainCamera.forward, glm::normalize(-mainCamera.pos - sunpos));
	
	if (looksun > 0)
	{
		meshManager.setShaderConst(lightShaftQuad, "pass", 0.f, 0.f, 0.f, 0.f);
		meshManager.setShaderConst(lightShaftQuad, "lightpos", sunpos2d.x, sunpos2d.y, looksun, 0.f);
		meshManager.setShaderConst(lightShaftQuad, "resolution", res.x, res.y, 0, 0);

		meshManager.setTexture(lightShaftQuad, renderImg[0].depth, 0);
		//meshManager.setTexture(lightShaftQuad, lightShaftBuffer[0].color, 1);
		
		drawMesh(&meshManager.meshArray[lightShaftQuad]);

		// downsample light shafts
		setRenderToTexture(lightShaftBuffer[2], 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		meshManager.setTexture(lightShaftQuad, lightShaftBuffer[0].color, 1);
		meshManager.setShaderConst(lightShaftQuad, "pass", 1, 0, 0, 0);

		drawMesh(&meshManager.meshArray[lightShaftQuad]);

		// produce light shafts
		setRenderToTexture(lightShaftBuffer[1], 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		meshManager.setTexture(lightShaftQuad, lightShaftBuffer[2].color, 1);
		meshManager.setShaderConst(lightShaftQuad, "pass", 2.f, 0.f, 0.f, 0.f);

		drawMesh(&meshManager.meshArray[lightShaftQuad]);

		


		blurBuffer(lightShaftBuffer[1].color, lightShaftBuffer[1], 3, 1.0f);


	}
	
	

	// update light sample of currentview;
	if (++apertureAdjustTimer > 3)
	{
		sampleLight(renderImg[0]);
		apertureAdjustTimer = 0;
	}
	
	// create bloom effect
	extractHighlights(renderImg[0].color);
	blurBuffer(bloomBuffer[1].color, bloomBuffer[1], 10, 4.0f);
	
	setRenderToScreen();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawMesh(&meshManager.meshArray[renderQuad]);
	
	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LESS);

	for (int n = 0; n < meshManager.meshCount; n++)
	{
		if (meshManager.meshArray[n].type == EDITOR)
		{
			drawMesh(&meshManager.meshArray[n]);
		}
	}

	// render Text
	char text[256];
	sprintf_s(text, "%.04f", getFrameRate());
	textManager.textArray[editor.fpsText].pos = glm::vec2(10, windowSize.y - 36);
	textManager.setString(editor.fpsText, text);

	
	// draw 2d items (sprite & text) in order of depth - far to near
	glEnable(GL_BLEND);
	if (samples > 0)
	{
		glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		glDisable(GL_SAMPLE_ALPHA_TO_ONE);
	}
	glDepthFunc(GL_ALWAYS);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	sort2DItems();
	for (int n = 0; n < sorting2DCount; n++)
	{
		if (sorting2DArray[n].type == SortItem2D::SPRITE)
		{
			if (spriteManager.getVisible(sorting2DArray[n].arrayIndex))
			{
				if (spriteManager.getType(sorting2DArray[n].arrayIndex) != spriteManager.EDITOR) drawSprite(&spriteManager.spriteArray[sorting2DArray[n].arrayIndex]);
			}
		}
		if (sorting2DArray[n].type == SortItem2D::TEXT)
		{
			if (textManager.getVisible(sorting2DArray[n].arrayIndex))
			{
				if (textManager.getType(sorting2DArray[n].arrayIndex) != textManager.EDITOR) drawText(sorting2DArray[n].arrayIndex);
			}
		}
	}

	// draw editor entities
	if (editor.editorMode)
	{
		while (editor.drawReqStack.size > 0)
		{
			DrawRequest a = editor.drawReqStack.pop();
			if (a.type == editor.EDITOR_SPRITE)
				drawSprite(&spriteManager.spriteArray[a.index]);
			if (a.type == editor.EDITOR_TEXT)
				drawText(a.index);
			if (a.type == editor.EDITOR_GEOMETRY)
				drawMesh(&meshManager.meshArray[a.index]);
		}
	}
	
	glDisable(GL_BLEND);
	glDepthFunc(GL_LESS);



	// Swap buffers
	if (VSyncState)
		glFinish();

	glfwSwapBuffers(window);
	glfwPollEvents();

	
	return;
}

void Engine::drawMesh(MeshObj *current)
{

	
	if (current->visible)
	{
		
		if (current->culling == 0)
		{
			glDisable(GL_CULL_FACE);
			glCullFace(GL_NONE);
			}
		if (current->culling == 1)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
		if (current->culling == 2)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
		}

		

		if (shaderState->id != shaderManager.shaderArray[current->shaderIndex].id)
		{
			glUseProgram(shaderManager.shaderArray[current->shaderIndex].id);
			shaderState = &shaderManager.shaderArray[current->shaderIndex];
		}

	
		currentModel = &current->modelMatrix;
		mvp = view_projMatrix * *currentModel;

		current->setShaderMat(shaderState, "MVP", mvp);
		current->setShaderMat(shaderState, "transform", current->transformMatrix);
		current->setShaderMat(shaderState, "model", current->modelMatrix);

		//do this if mesh receives shadows 
		mvp_shadow = shadowViewProj[0] * *currentModel;
		current->setShaderMat(shaderState, "MVP_shadow0", mvp_shadow);
		mvp_shadow = shadowViewProj[1] * *currentModel;
		current->setShaderMat(shaderState, "MVP_shadow1", mvp_shadow);
		mvp_shadow = shadowViewProj[2] * *currentModel;
		current->setShaderMat(shaderState, "MVP_shadow2", mvp_shadow);
		mvp_shadow = shadowViewProj[3] * *currentModel;
		current->setShaderMat(shaderState, "MVP_shadow3", mvp_shadow);

		current->updateShaderUniforms(shaderState);
		// set all shader constants for this mesh

		if (current->texture[0] >= 0)
		{
			if (textureStateID[0] != current->texture[0])
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, current->texture[0]);
				glUniform1i(shaderState->textureLocation[0], 0);
				textureStateID[0] = current->texture[0];
			}
		}

		if (current->texture[1] >= 0)
		{
			if (textureStateID[1] != current->texture[1])
			{
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, current->texture[1]);
				glUniform1i(shaderState->textureLocation[1], 1);
				textureStateID[1] = current->texture[1];
			}
		}
		if (current->texture[2] >= 0)
		{
			if (textureStateID[2] != current->texture[2])
			{
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, current->texture[2]);
				glUniform1i(shaderState->textureLocation[2], 2);
				textureStateID[2] = current->texture[2];
			}
		}
		if (current->texture[3] >= 0)
		{
			if (textureStateID[3] != current->texture[3])
			{
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, current->texture[3]);
				glUniform1i(shaderState->textureLocation[3], 3);
				textureStateID[3] = current->texture[3];
			}
		}

		if (current->texture[4] >= 0)
		{
			if (textureStateID[4] != current->texture[4])
			{
				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, current->texture[4]);
				glUniform1i(shaderState->textureLocation[4], 4);
				textureStateID[4] = current->texture[4];
			}
		}

		if (current->texture[5] >= 0)
		{
			if (textureStateID[5] != current->texture[5])
			{
				glActiveTexture(GL_TEXTURE5);
				glBindTexture(GL_TEXTURE_2D, current->texture[5]);
				glUniform1i(shaderState->textureLocation[5], 5);
				textureStateID[5] = current->texture[5];
			}
		}

		if (current->texture[6] >= 0)
		{
			if (textureStateID[6] != current->texture[6])
			{
				glActiveTexture(GL_TEXTURE6);
				glBindTexture(GL_TEXTURE_2D, current->texture[6]);
				glUniform1i(shaderState->textureLocation[6], 6);
				textureStateID[6] = current->texture[6];
			}
		}

		if (current->texture[7] >= 0)
		{
			if (textureStateID[7] != current->texture[7])
			{
				glActiveTexture(GL_TEXTURE7);
				glBindTexture(GL_TEXTURE_2D, current->texture[7]);
				glUniform1i(shaderState->textureLocation[7], 7);
				textureStateID[7] = current->texture[7];
			}
		}

		if (current->texture[10] >= 0)
		{
			if (textureStateID[10] != current->texture[10])
			{
				glActiveTexture(GL_TEXTURE10);
				glBindTexture(GL_TEXTURE_2D, current->texture[10]);
				glUniform1i(shaderState->textureLocation[10], 10);
				textureStateID[10] = current->texture[10];
			}
		}
		
		if (current->texture[11] >= 0)
		{
			if (textureStateID[11] != current->texture[11])
			{
				glActiveTexture(GL_TEXTURE11);
				glBindTexture(GL_TEXTURE_2D, current->texture[11]);
				glUniform1i(shaderState->textureLocation[11], 11);
				textureStateID[11] = current->texture[11];
			}
		}

		if (current->texture[12] >= 0)
		{
			if (textureStateID[12] != current->texture[12])
			{
				glActiveTexture(GL_TEXTURE12);
				glBindTexture(GL_TEXTURE_2D, current->texture[12]);
				glUniform1i(shaderState->textureLocation[12], 12);
				textureStateID[12] = current->texture[12];
			}
		}

		if (current->texture[14] >= 0)
		{
			if (textureStateID[14] != current->texture[14])
			{
				glActiveTexture(GL_TEXTURE14);
				glBindTexture(GL_TEXTURE_2D, current->texture[14]);
				glUniform1i(shaderState->textureLocation[14], 14);
				textureStateID[14] = current->texture[14];
			}
		}

		if (current->texture[15] >= 0)
		{
			if (textureStateID[15] != current->texture[15])
			{
				glActiveTexture(GL_TEXTURE15);
				glBindTexture(GL_TEXTURE_2D, current->texture[15]);
				glUniform1i(shaderState->textureLocation[15], 15);
				textureStateID[15] = current->texture[15];
			}
		}

		if (current->texture[16] >= 0)
		{
			if (textureStateID[16] != current->texture[16])
			{
				glActiveTexture(GL_TEXTURE16);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, current->texture[16]);
				glUniform1i(shaderState->textureLocation[16], 16);
				textureStateID[16] = current->texture[16];
			}
		}


		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, current->vertexBuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. 
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

		if (current->type == STANDARD)
		{
			// 2nd attribute buffer : uv
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, current->uvBuffer);
			glVertexAttribPointer(
				1,                                // attribute. 
				2,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			// 3nd attribute buffer : normals
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, current->normalBuffer);
			glVertexAttribPointer(
				2,                                // attribute. 
				3,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			
			if (current->useVertexLight)
			{
				
				// vertex lighting
				glEnableVertexAttribArray(3);
				glBindBuffer(GL_ARRAY_BUFFER, current->vertGIBuffer);
				glVertexAttribPointer(
					3,                                // attribute. 
					3,                                // size
					GL_FLOAT,                         // type
					GL_FALSE,                         // normalized?
					0,                                // stride
					(void*)0                          // array buffer offset
				);
			}

			if (current->useTangents)
			{
				// tangents
				glEnableVertexAttribArray(4);
				glBindBuffer(GL_ARRAY_BUFFER, current->tangentBuffer);
				glVertexAttribPointer(
					4,                                // attribute. 
					3,                                // size
					GL_FLOAT,                         // type
					GL_FALSE,                         // normalized?
					0,                                // stride
					(void*)0                          // array buffer offset
				);

				// binormals
				glEnableVertexAttribArray(5);
				glBindBuffer(GL_ARRAY_BUFFER, current->binormalBuffer);
				glVertexAttribPointer(
					5,                                // attribute. 
					3,                                // size
					GL_FLOAT,                         // type
					GL_FALSE,                         // normalized?
					0,                                // stride
					(void*)0                          // array buffer offset
				);
			}
			
			// Index buffer
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current->elementBuffer);
			glDisable(GL_LINE_SMOOTH);
			
			


			if (drawWireFrame)
			{
				glEnable(GL_LINE_SMOOTH);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}

			glDrawElements(
				GL_TRIANGLES,      // mode
				current->indices.size,    // count
				GL_UNSIGNED_INT,   // type
				(void*)0           // element array buffer offset
			);

			
			//glPolygonMode(GL_FRONT, GL_FILL);

			
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
			
			if (current->useVertexLight)
			{
				glDisableVertexAttribArray(3);
			}
			if (current->useTangents)
			{
				glDisableVertexAttribArray(4);
				glDisableVertexAttribArray(5);
			}
		}

		if (current->type == QUAD)
		{
			glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles

			glDisableVertexAttribArray(0);
		}
	}
}

void Engine::drawMeshGI(MeshObj *current)
{

	if (current->visible)
	{
		glDisable(GL_CULL_FACE);

		if (shaderState->id != shaderManager.shaderArray[current->shaderIndex].id)
		{
			glUseProgram(shaderManager.shaderArray[current->shaderIndex].id);
			shaderState = &shaderManager.shaderArray[current->shaderIndex];
		}


		currentModel = &current->modelMatrix;
		mvp = view_projMatrix * *currentModel;

		current->setShaderMat(shaderState, "MVP", mvp);
		current->setShaderMat(shaderState, "transform", current->transformMatrix);
		current->setShaderMat(shaderState, "model", current->modelMatrix);

		//do this if mesh receives shadows 
		mvp_shadow = shadowViewProj[0] * *currentModel;
		current->setShaderMat(shaderState, "MVP_shadow0", mvp_shadow);
		mvp_shadow = shadowViewProj[1] * *currentModel;
		current->setShaderMat(shaderState, "MVP_shadow1", mvp_shadow);
		mvp_shadow = shadowViewProj[2] * *currentModel;
		current->setShaderMat(shaderState, "MVP_shadow2", mvp_shadow);
		mvp_shadow = shadowViewProj[3] * *currentModel;
		current->setShaderMat(shaderState, "MVP_shadow3", mvp_shadow);

		current->updateShaderUniforms(shaderState);
		// set all shader constants for this mesh

		if (current->texture[0] >= 0)
		{
			if (textureStateID[0] != current->texture[0])
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, current->texture[0]);
				glUniform1i(shaderState->textureLocation[0], 0);
				textureStateID[0] = current->texture[0];
			}
		}

		if (current->texture[1] >= 0)
		{
			if (textureStateID[1] != current->texture[1])
			{
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, current->texture[1]);
				glUniform1i(shaderState->textureLocation[1], 1);
				textureStateID[1] = current->texture[1];
			}
		}
		if (current->texture[2] >= 0)
		{
			if (textureStateID[2] != current->texture[2])
			{
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, current->texture[2]);
				glUniform1i(shaderState->textureLocation[2], 2);
				textureStateID[2] = current->texture[2];
			}
		}
		if (current->texture[3] >= 0)
		{
			if (textureStateID[3] != current->texture[3])
			{
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, current->texture[3]);
				glUniform1i(shaderState->textureLocation[3], 3);
				textureStateID[3] = current->texture[3];
			}
		}

		if (current->texture[4] >= 0)
		{
			if (textureStateID[4] != current->texture[4])
			{
				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, current->texture[4]);
				glUniform1i(shaderState->textureLocation[4], 4);
				textureStateID[4] = current->texture[4];
			}
		}

		if (current->texture[5] >= 0)
		{
			if (textureStateID[5] != current->texture[5])
			{
				glActiveTexture(GL_TEXTURE5);
				glBindTexture(GL_TEXTURE_2D, current->texture[5]);
				glUniform1i(shaderState->textureLocation[5], 5);
				textureStateID[5] = current->texture[5];
			}
		}

		if (current->texture[6] >= 0)
		{
			if (textureStateID[6] != current->texture[6])
			{
				glActiveTexture(GL_TEXTURE6);
				glBindTexture(GL_TEXTURE_2D, current->texture[6]);
				glUniform1i(shaderState->textureLocation[6], 6);
				textureStateID[6] = current->texture[6];
			}
		}

		if (current->texture[7] >= 0)
		{
			if (textureStateID[7] != current->texture[7])
			{
				glActiveTexture(GL_TEXTURE7);
				glBindTexture(GL_TEXTURE_2D, current->texture[7]);
				glUniform1i(shaderState->textureLocation[7], 7);
				textureStateID[7] = current->texture[7];
			}
		}

		if (current->texture[10] >= 0)
		{
			if (textureStateID[10] != current->texture[10])
			{
				glActiveTexture(GL_TEXTURE10);
				glBindTexture(GL_TEXTURE_2D, current->texture[10]);
				glUniform1i(shaderState->textureLocation[10], 10);
				textureStateID[10] = current->texture[10];
			}
		}

		if (current->texture[11] >= 0)
		{
			if (textureStateID[11] != current->texture[11])
			{
				glActiveTexture(GL_TEXTURE11);
				glBindTexture(GL_TEXTURE_2D, current->texture[11]);
				glUniform1i(shaderState->textureLocation[11], 11);
				textureStateID[11] = current->texture[11];
			}
		}

		if (current->texture[12] >= 0)
		{
			if (textureStateID[12] != current->texture[12])
			{
				glActiveTexture(GL_TEXTURE12);
				glBindTexture(GL_TEXTURE_2D, current->texture[12]);
				glUniform1i(shaderState->textureLocation[12], 12);
				textureStateID[12] = current->texture[12];
			}
		}

		if (current->texture[14] >= 0)
		{
			if (textureStateID[14] != current->texture[14])
			{
				glActiveTexture(GL_TEXTURE14);
				glBindTexture(GL_TEXTURE_2D, current->texture[14]);
				glUniform1i(shaderState->textureLocation[14], 14);
				textureStateID[14] = current->texture[14];
			}
		}

		if (current->texture[15] >= 0)
		{
			if (textureStateID[15] != current->texture[15])
			{
				glActiveTexture(GL_TEXTURE15);
				glBindTexture(GL_TEXTURE_2D, current->texture[15]);
				glUniform1i(shaderState->textureLocation[15], 15);
				textureStateID[15] = current->texture[15];
			}
		}

		if (current->texture[16] >= 0)
		{
			if (textureStateID[16] != current->texture[16])
			{
				glActiveTexture(GL_TEXTURE16);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, current->texture[16]);
				glUniform1i(shaderState->textureLocation[16], 16);
				textureStateID[16] = current->texture[16];
			}
		}


		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, current->vertexBuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. 
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		if (current->type == STANDARD)
		{
			// 2nd attribute buffer : uv
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, current->uvBuffer);
			glVertexAttribPointer(
				1,                                // attribute. 
				2,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			// 3nd attribute buffer : normals
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, current->normalBuffer);
			glVertexAttribPointer(
				2,                                // attribute. 
				3,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);


			if (current->useVertexLight)
			{

				// vertex lighting
				glEnableVertexAttribArray(3);
				glBindBuffer(GL_ARRAY_BUFFER, current->vertGIBuffer);
				glVertexAttribPointer(
					3,                                // attribute. 
					3,                                // size
					GL_FLOAT,                         // type
					GL_FALSE,                         // normalized?
					0,                                // stride
					(void*)0                          // array buffer offset
				);
			}
			
			if (current->useTangents)
			{
				// tangents
				glEnableVertexAttribArray(4);
				glBindBuffer(GL_ARRAY_BUFFER, current->tangentBuffer);
				glVertexAttribPointer(
					4,                                // attribute. 
					3,                                // size
					GL_FLOAT,                         // type
					GL_FALSE,                         // normalized?
					0,                                // stride
					(void*)0                          // array buffer offset
				);

				// binormals
				glEnableVertexAttribArray(5);
				glBindBuffer(GL_ARRAY_BUFFER, current->binormalBuffer);
				glVertexAttribPointer(
					5,                                // attribute. 
					3,                                // size
					GL_FLOAT,                         // type
					GL_FALSE,                         // normalized?
					0,                                // stride
					(void*)0                          // array buffer offset
				);
			}
			
			// Index buffer
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current->elementBuffer);

			if (drawWireFrame)
			{
				glEnable(GL_LINE_SMOOTH);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}

			glDrawElements(
				GL_TRIANGLES,      // mode
				current->indices.size,    // count
				GL_UNSIGNED_INT,   // type
				(void*)0           // element array buffer offset
			);

			glPolygonMode(GL_FRONT, GL_FILL);
			;
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
			if (current->useVertexLight)
			{
				glDisableVertexAttribArray(3);
			}
			if (current->useTangents)
			{
				glDisableVertexAttribArray(4);
				glDisableVertexAttribArray(5);
			}
		}
		if (current->type == QUAD)
		{
			glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles

			glDisableVertexAttribArray(0);
		}
	}
}

void Engine::drawParticle(ParticleSet *current)
{

	if (shaderState->id != shaderManager.shaderArray[current->shaderIndex].id)
	{
		glUseProgram(shaderManager.shaderArray[current->shaderIndex].id);
		shaderState = &shaderManager.shaderArray[current->shaderIndex];
	}

	if (textureStateID[0] != current->texture[0])
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, current->texture[0]);
		glUniform1i(shaderState->textureLocation[0], 0);
		textureStateID[0] = current->texture[0];
	}

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, current->texture[1]);

	//glDisable(GL_CULL_FACE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	current->setShaderMat(shaderState, "MVP", view_projMatrix);

	current->updateShaderUniforms(shaderState);

	

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, meshManager.quad_vertexbuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0,  (void*)0 );

	// position buffer
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, current->posBufferID);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribDivisor(1, 1);

	// rotation buffer
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, current->rotBufferID);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribDivisor(2, 1);

	// scale buffer
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, current->scaleBufferID);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribDivisor(3, 1);

	// color buffer
	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, current->colorBufferID);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribDivisor(4, 1);
	
	glEnable(GL_BLEND);
	glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
	glDisable(GL_SAMPLE_ALPHA_TO_ONE);

	if (current->blendType == 0) glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (current->blendType == 1) glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	
	glDepthMask(false);

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, current->count);

	glDepthMask(true);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_BLEND);

	glVertexAttribDivisor(1, 0);
	glVertexAttribDivisor(2, 0);
	glVertexAttribDivisor(3, 0);
	glVertexAttribDivisor(4, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);

}

void Engine::drawMeshShadowMap(MeshObj *current, int cascade)
{
	if (current->type == QUAD) return;

	if (current->visible)
	{
		if (current->culling == 0)
			glDisable(GL_CULL_FACE);
		if (current->culling == 1)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
		if (current->culling == 2)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
		}

		if (shaderState->id != shaderManager.shaderArray[defaultShadowShader].id)
		{
			glUseProgram(shaderManager.shaderArray[defaultShadowShader].id);
			shaderState = &shaderManager.shaderArray[defaultShadowShader];
		}

		currentModel = &current->modelMatrix;
		mvp = shadowViewProj[cascade] * *currentModel;


		current->setShaderMat(shaderState, "MVP", mvp);
		current->setShaderMat(shaderState, "transform", current->transformMatrix);
		current->setShaderMat(shaderState, "model", current->modelMatrix);



		current->updateShaderUniforms(shaderState);
		// set all shader constants for this mesh


		if (current->texture[0] >= 0)
		{
			if (textureStateID[0] != current->texture[0])
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, current->texture[0]);
				glUniform1i(shaderState->textureLocation[0], 0);
				textureStateID[0] = current->texture[0];
			}
		}


		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, current->vertexBuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

		// 2nd attribute buffer : uv
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, current->uvBuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// 3nd attribute buffer : uvs
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, current->normalBuffer);
		glVertexAttribPointer(
			2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		//glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
		//glUniformMatrix4fv(TransformMatrixID, 1, GL_FALSE, &current->transformMatrix[0][0]);
		//glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &current->modelMatrix[0][0]);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current->elementBuffer);
		//glDisable(GL_POLYGON_SMOOTH);
		//glDisable(GL_LINE_SMOOTH);
		

		glDrawElements(
			GL_TRIANGLES,      // mode
			current->indices.size,    // count
			GL_UNSIGNED_INT,   // type
			(void*)0           // element array buffer offset
			);


	//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}
}

void Engine::drawSprite(Sprite *current)
{
	if (shaderState->id != current->shader->id)
	{
		glUseProgram(current->shader->id);
		shaderState = current->shader;
	}

	current->setShaderConst("resolution", windowSize.x, windowSize.y, 0, 0);
	current->updateShaderUniforms();

	if (textureStateID[0] != current->texture[0])
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, current->texture[0]);
		glUniform1i(current->shader->textureLocation[0], 0);
		textureStateID[0] = current->texture[0];
	}

	if (textureStateID[1] != current->texture[1])
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, current->texture[1]);
		glUniform1i(current->shader->textureLocation[1], 1);
		textureStateID[1] = current->texture[1];
	}

	if (textureStateID[2] != current->texture[2])
	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, current->texture[2]);
		glUniform1i(current->shader->textureLocation[2], 2);
		textureStateID[2] = current->texture[2];
	}

	if (textureStateID[3] != current->texture[3])
	{
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, current->texture[3]);
		glUniform1i(current->shader->textureLocation[3], 3);
		textureStateID[3] = current->texture[3];
	}


	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, current->vertBufferID);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, spriteManager.uvBufferID);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);


	// Draw the triangles
	glDrawArrays(GL_TRIANGLES, 0, 6); 

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void Engine::drawSelectedMesh(MeshObj *current)
{
	if (current->visible)
	{

		glDisable(GL_CULL_FACE);
		//glCullFace(GL_BACK);

		if (shaderState->id != shaderManager.shaderArray[editorSelectShader].id)
		{
			glUseProgram(shaderManager.shaderArray[editorSelectShader].id);
			shaderState = &shaderManager.shaderArray[editorSelectShader];
		}


		currentModel = &current->modelMatrix;
		mvp = view_projMatrix * *currentModel;


		current->setShaderMat(shaderState, "MVP", mvp);
		current->setShaderMat(shaderState, "transform", current->transformMatrix);
		current->setShaderMat(shaderState, "model", current->modelMatrix);

		current->updateShaderUniforms(shaderState);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, current->vertexBuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

		// 2nd attribute buffer : uv
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, current->uvBuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// 3nd attribute buffer : uvs
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, current->normalBuffer);
		glVertexAttribPointer(
			2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current->elementBuffer);
		

		glEnable(GL_LINE_SMOOTH);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


		glDrawElements(
			GL_TRIANGLES,      // mode
			current->indices.size,    // count
			GL_UNSIGNED_INT,   // type
			(void*)0           // element array buffer offset
			);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}
}

void Engine::drawText(int index)
{
	glCullFace(GL_BACK);

	TextObj *current = &textManager.textArray[index];
	Font *currentFont = &textManager.fontArray[textManager.textArray[index].fontID];
	short int length = strnlen_s(current->str, 30);

	float xpos = current->pos.x;
	float ypos = current->pos.y;
	float scale = current->size / currentFont->characterSize[current->str[0]].y;


	// Fill buffers
	for (unsigned short i = 0; i<length; i++)
	{

		textManager.vertex_up_left = glm::vec2(xpos, ypos + current->size);
		textManager.vertex_up_right = glm::vec2(xpos + currentFont->characterSize[current->str[i]].x *scale, ypos + current->size);
		textManager.vertex_down_right = glm::vec2(xpos + currentFont->characterSize[current->str[i]].x*scale, ypos);
		textManager.vertex_down_left = glm::vec2(xpos, ypos);

		xpos += currentFont->characterSize[current->str[i]].x*scale;

		textManager.vertices[i * 6] = textManager.vertex_up_left;


		textManager.vertices[i * 6 + 1] = textManager.vertex_down_left;
		textManager.vertices[i * 6 + 2] = textManager.vertex_up_right;
		textManager.vertices[i * 6 + 3] = textManager.vertex_down_right;
		textManager.vertices[i * 6 + 4] = textManager.vertex_up_right;
		textManager.vertices[i * 6 + 5] = textManager.vertex_down_left;

		textManager.character = current->str[i];

		glm::vec2 pos;
		glm::vec2 csize;
		pos = currentFont->characterPos[textManager.character];
		csize = currentFont->characterSize[textManager.character];

		glm::vec2 fontImgSize = currentFont->fontImg.size;
		textManager.uv_up_left = glm::vec2(pos.x, pos.y + csize.y) / float(fontImgSize.x);
		textManager.uv_up_right = glm::vec2(pos.x + csize.x, pos.y + csize.y) / float(fontImgSize.x);
		textManager.uv_down_right = glm::vec2(pos.x + csize.x, pos.y) / float(fontImgSize.y);
		textManager.uv_down_left = glm::vec2(pos.x, pos.y) / float(fontImgSize.y);


		textManager.UVs[i * 6] = textManager.uv_up_left;
		textManager.UVs[i * 6 + 1] = textManager.uv_down_left;
		textManager.UVs[i * 6 + 2] = textManager.uv_up_right;

		textManager.UVs[i * 6 + 3] = textManager.uv_down_right;
		textManager.UVs[i * 6 + 4] = textManager.uv_up_right;
		textManager.UVs[i * 6 + 5] = textManager.uv_down_left;
	}

	glBindBuffer(GL_ARRAY_BUFFER, textManager.Text2DVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, length * 6 * sizeof(glm::vec2), &textManager.vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, textManager.Text2DUVBufferID);
	glBufferData(GL_ARRAY_BUFFER, length * 6 * sizeof(glm::vec2), &textManager.UVs[0], GL_STATIC_DRAW);

	// Bind shader
	if (shaderState->id != textManager.textShaderID)
	{
		glUseProgram(textManager.textShaderID);
		shaderState = textManager.shader;
	}

	textManager.setShaderConst(index, "resolution", windowSize.x, windowSize.y, 0, 0);
	textManager.updateShaderUniforms(index);

	// Bind texture
	if (currentFont->fontImg.id >= 0)
	{
		if (textureStateID[0] != currentFont->fontImg.id)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, currentFont->fontImg.id);
			glUniform1i(textManager.shader->textureLocation[0], 0);
			textureStateID[0] = currentFont->fontImg.id;
		}
	}

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, textManager.Text2DVertexBufferID);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, textManager.Text2DUVBufferID);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Draw call
	glDrawArrays(GL_TRIANGLES, 0, length * 6);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void Engine::blurBuffer(GLuint input, RenderTexture output, int kernel, float spread)
{
	//glEnable(GL_BLEND);
	setRenderToTexture(convolutionBuffer[0], 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	meshManager.setTexture(blurQuad, input, 0);
	meshManager.setShaderConst(blurQuad, "blurScale", 4.0f / res.x, 0.f, 0.f, 0.f);
	meshManager.setShaderConst(blurQuad, "kernel", kernel, 0, 0, 0);

	drawMesh(&meshManager.meshArray[blurQuad]);

	setRenderToTexture(output, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	meshManager.setTexture(blurQuad, convolutionBuffer[0].color, 0);
	meshManager.setShaderConst(blurQuad, "blurScale", 0.0f, 4.0f / res.y, 0.f, 0.f);
	meshManager.setShaderConst(blurQuad, "kernel", kernel, 0, 0, 0);

	drawMesh(&meshManager.meshArray[blurQuad]);
	//glDisable(GL_BLEND);
	
	return;
}

void Engine::colorGrading(GLuint input, RenderTexture output)
{
	setRenderToTexture(output, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	meshManager.setTexture(colorGradeQuad, input, 0);

	drawMesh(&meshManager.meshArray[colorGradeQuad]);
}

void Engine::extractHighlights(GLuint input)
{
	
	setRenderToTexture(bloomBuffer[0], 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	meshManager.setShaderConst(bloomQuad, "pass", 0.f, 0.f, 0.f, 0.f);
	meshManager.setShaderConst(bloomQuad, "resolution", res.x/2.0f, res.y/2.0f, 0.f, 0.f);
	meshManager.setTexture(bloomQuad, input, 0);

	drawMesh(&meshManager.meshArray[bloomQuad]);

	setRenderToTexture(bloomBuffer[1], 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	meshManager.setShaderConst(bloomQuad, "pass", 1.f, 0.f, 0.f, 0.f);
	meshManager.setShaderConst(bloomQuad, "resolution", res.x / 4.0f, res.y / 4.0f, 0.f, 0.f);
	meshManager.setTexture(bloomQuad, bloomBuffer[0].color, 0);

	drawMesh(&meshManager.meshArray[bloomQuad]);

	return;
}

// antialias using FXAA
void Engine::antialiasBuffer(RenderTexture input)
{

	setRenderToTexture(FXAA_lumaBuffer, 0);
	//glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	//glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
	//glDisable(GL_SAMPLE_ALPHA_TO_ONE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	meshManager.setShader(FXAA_Quad, FXAA_lumaShader);
	meshManager.setTexture(FXAA_Quad, input.color, 0);
	drawMesh(&meshManager.meshArray[FXAA_Quad]);

	setRenderToTexture(input, -1);
	glClear(GL_COLOR_BUFFER_BIT);
	
	meshManager.setShader(FXAA_Quad, FXAA_Shader);
	meshManager.setTexture(FXAA_Quad, FXAA_lumaBuffer.color, 0);
	meshManager.setShaderConst(FXAA_Quad, "resolution", input.size.x, input.size.y, 0, 0);

	drawMesh(&meshManager.meshArray[FXAA_Quad]);

	return;
}

void Engine::sampleLight(RenderTexture input)
{
	setRenderToTexture(lightSampleImg, 0);
	//glClear(GL_DEPTH_BUFFER_BIT);
	glDepthMask(false);
	glDepthFunc(GL_ALWAYS);
	meshManager.setTexture(lightSampleQuad, input.color, 0);
	
	if (samples > 0)
	{
		glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		glDisable(GL_SAMPLE_ALPHA_TO_ONE);
	}
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);
	drawMesh(&meshManager.meshArray[lightSampleQuad]);

	glActiveTexture(GL_TEXTURE16);
	glBindTexture(GL_TEXTURE_2D, lightSampleImg.colorID);
	glGenerateMipmap(GL_TEXTURE_2D);

	glDisable(GL_BLEND);
	glDepthMask(true);
	return;
}

glm::vec3 Engine::screenPointToVector(glm::vec2 point)
{
	glm::mat4 view = glm::lookAt(
		glm::vec3(0),
		control.direction, 
		control.up  
		);
	glm::mat4 view_projMatrixInverse = glm::inverse(mainCamera.projMatrix * view);
	glm::vec4 direction = view_projMatrixInverse * glm::vec4(point.x, point.y, 0, 1);
	return glm::vec3(direction.x, direction.y, direction.z);
}

glm::vec2 Engine::pointToScreenPos(glm::vec3 point)
{

	//glm::mat4 view = viewMatrix;
	//glm::mat4 proj = projectionMatrix;// glm::perspective(glm::radians(mainCamera.FOV), 16.f / 9.f, .1f, 1000.f);
	
	//glm::mat4 vpm = proj*view;
	glm::vec4 pos = mainCamera.view_projMatrix * glm::vec4(point.x, point.y, point.z, 1);

	pos.x /= pos.w;
	pos.y /= pos.w;

	return (glm::vec2(pos.x, pos.y) + 1.0f) * glm::vec2(res) / 2.0f;
}

glm::vec3 Engine::screenPointToVector(glm::vec2 point, Camera cam)
{
	glm::mat4 view = glm::lookAt(
		glm::vec3(0),
		cam.forward,
		cam.up
	);
	glm::mat4 view_projMatrixInverse = glm::inverse(cam.projMatrix * view);
	glm::vec4 direction = view_projMatrixInverse * glm::vec4(point.x, point.y, 0, 1);
	return glm::vec3(direction.x, direction.y, direction.z);
}

glm::vec2 Engine::pointToScreenPos(glm::vec3 point, Camera cam, glm::vec2 resolution)
{
	glm::vec4 pos = cam.view_projMatrix * glm::vec4(point.x, point.y, point.z, 1);

	pos.x /= pos.w;
	pos.y /= pos.w;

	return (glm::vec2(pos.x, pos.y) + 1.f)*resolution / 2.f;
}

void Engine::frustumCull(Camera cam)
{

	glm::vec4 p0, p1;
	glm::vec3 maxpos, minpos;

	for (int n = 0; n < meshManager.meshCount; n++)
	{
		if (meshManager.meshArray[n].type != QUAD && meshManager.meshArray[n].type != EDITOR)
		{

			//meshManager.meshArray[n].updateModelMatrix();
			p0 = meshManager.meshArray[n].modelMatrix * glm::vec4(meshManager.meshArray[n].maxpos, 1);
			p1 = meshManager.meshArray[n].modelMatrix * glm::vec4(meshManager.meshArray[n].minpos, 1);

			maxpos = glm::vec3(p0.x, p0.y, p0.z);
			minpos = glm::vec3(p1.x, p1.y, p1.z);

			frustumCull_vert[0] = glm::vec3(maxpos.x, minpos.y, minpos.z);
			frustumCull_vert[1] = glm::vec3(maxpos.x, minpos.y, maxpos.z);
			frustumCull_vert[2] = glm::vec3(minpos.x, minpos.y, maxpos.z);
			frustumCull_vert[3] = glm::vec3(minpos.x, minpos.y, minpos.z);
			frustumCull_vert[4] = glm::vec3(maxpos.x, maxpos.y, minpos.z);
			frustumCull_vert[5] = glm::vec3(maxpos.x, maxpos.y, maxpos.z);
			frustumCull_vert[6] = glm::vec3(minpos.x, maxpos.y, maxpos.z);
			frustumCull_vert[7] = glm::vec3(minpos.x, maxpos.y, minpos.z);

			for (int c = 0; c < 8; c++)
			{
				frustumCull_check[c] = (glm::dot(cam.forward, glm::normalize(frustumCull_vert[c]-cam.pos)) > 0);
			}

			meshManager.meshArray[n].frustumCulled = false;
			if (!frustumCull_check[0] && !frustumCull_check[1] && !frustumCull_check[2] && !frustumCull_check[3] && !frustumCull_check[4] && !frustumCull_check[5] && !frustumCull_check[6] && !frustumCull_check[7])
			{
				meshManager.meshArray[n].frustumCulled = true;
				continue;
			}

			// 0 = in frustum, 1 = left of, 2 = right of
			// 0 = in, 1 = above, 2 = below;
			// 3 = behind
			for (int c = 0; c < 8; c++)
			{
				frustumCull_pos = cam.view_projMatrix * glm::vec4(frustumCull_vert[c].x, frustumCull_vert[c].y, frustumCull_vert[c].z, 1);

				frustumCull_pos.x /= frustumCull_pos.w;
				frustumCull_pos.y /= frustumCull_pos.w;

				frustumCull_state[c] = glm::ivec2(0);
				if (frustumCull_pos.x < -1) frustumCull_state[c].x = 1;
				if (frustumCull_pos.x > 1) frustumCull_state[c].x = 2;
				if (frustumCull_pos.y < -1) frustumCull_state[c].y = 1;
				if (frustumCull_pos.y > 1) frustumCull_state[c].y = 2;

				if (!frustumCull_check[c])
				{
					
					{
						if (glm::dot(glm::normalize(frustumCull_vert[c] - cam.pos), cam.right) >= 0)
							frustumCull_state[c].x = 4;
						else
							frustumCull_state[c].x = 3;
					}
					
					{
						if (glm::dot(glm::normalize(frustumCull_vert[c] - cam.pos), cam.up) >= 0)
							frustumCull_state[c].y = 4;
						else
							frustumCull_state[c].y = 3;
					}
				}
			}

			frustumCull_passx = false;
			frustumCull_passy = false;
			frustumCull_leftcount = 0;
			frustumCull_rightcount = 0;
			frustumCull_abovecount = 0;
			frustumCull_belowcount = 0;

			for (int c = 0; c < 8; c++)
			{
				//if (state[c].x != 3)
				{
					if (frustumCull_state[c].x == 0)
					{
						frustumCull_passx = true;
					}
					if (frustumCull_state[c].y == 0)
					{
						frustumCull_passy = true;
					}
					if (frustumCull_passx && frustumCull_passy) break;

					// left
					if (frustumCull_state[c].x == 1 || frustumCull_state[c].x == 3)
					{
						frustumCull_leftcount++;
					}

					// right;
					if (frustumCull_state[c].x == 2 || frustumCull_state[c].x == 4)
					{
						frustumCull_rightcount++;
					}

					// above
					if (frustumCull_state[c].y == 1 || frustumCull_state[c].y == 3)
					{
						frustumCull_abovecount++;
					}

					// below;
					if (frustumCull_state[c].y == 2 || frustumCull_state[c].y == 4)
					{
						frustumCull_belowcount++;
					}
				}
			}
			
			

			if (!frustumCull_passx)
			{
				if ((frustumCull_leftcount > 0 && frustumCull_rightcount == 0) || (frustumCull_leftcount == 0 && frustumCull_rightcount > 0))
				{
					meshManager.meshArray[n].frustumCulled = true;
					continue;
				}
			}
			if (!frustumCull_passy)
			{
				if ((frustumCull_abovecount > 0 && frustumCull_belowcount == 0) || (frustumCull_abovecount == 0 && frustumCull_belowcount > 0))
				{
					meshManager.meshArray[n].frustumCulled = true;
					continue;
				}
			}
			
		}
	}
}

// get window size
glm::ivec2 Engine::getWindowSize()
{
	glm::ivec2 sz;
	glfwGetWindowSize(window, &sz.x, &sz.y);

	return sz;
}

// set window size
void Engine::setWindowSize(int width, int height)
{
	glfwSetWindowSize(window, width, height);
}

// call every frame to track accurate frame draw times
void Engine::updateFrameTimeInfo()
{
	last_clock = clock;
	clock = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::duration<double>>(clock - last_clock);

	frameTime = duration.count();
}

// get time in milliseconds taken to render last frame
double Engine::getFrameTime()
{
	return frameTime;
}

// get approximated number of frames rendered per second
double Engine::getFrameRate()
{
	return 1.0 / getFrameTime();
}

// 
void Engine::setVSync(char state)
{
	wglSwapIntervalEXT(state);
	VSyncState = state;
}

// 
char* Engine::strToChar(std::string str)
{
	const int len = strlen(str.c_str());;
	
	char *temp = (char*)malloc((len+1)*sizeof(char));
	strcpy_s(temp, (len+1)*sizeof(char), str.c_str());

	return temp;
}

// get specific token from a char string
char* Engine::strToken(char* str,  char* delimiter, int token)
{
	char s1[1024];
	strcpy_s(s1, str);

	char *temp = NULL;
	char *nextToken = NULL;

	int count = glm::max(token, 0);
	temp = strtok_s(s1, delimiter, &nextToken);

	for (int n = 0; n < count; n++)
	{
		if (temp != NULL)
			temp = strtok_s(NULL, delimiter, &nextToken);
	}

	return temp;
}

int Engine::instanceMesh(int index)
{
	MeshObj temp = meshManager.meshArray[index].instanceCopy();
	temp.instanceOf = &meshManager.meshArray[index];


	return meshManager.add(temp);
}

void Engine::setRenderToTexture(RenderTexture rt, GLint depthTex)
{
	glBindFramebuffer(GL_FRAMEBUFFER, rt.fbo);
	if (depthTex > 0)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
	}
	
	if (depthTex == 0)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, rt.depthID, 0);
	}

	if (depthTex < 0)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
	}
	glViewport(0, 0, rt.size.x, rt.size.y);
}

void Engine::setRenderToScreen()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowSize.x, windowSize.y);
}

void Engine::resolveMultisample(RenderTexture MSbuffer, RenderTexture Nbuffer)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, MSbuffer.fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Nbuffer.fbo); // Normal FBO can be the default FBO too.

	glBlitFramebuffer(0, 0, MSbuffer.size.x, MSbuffer.size.y, 0, 0, Nbuffer.size.x, Nbuffer.size.y, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

void Engine::copyRenderBuffer(RenderTexture buffer0, RenderTexture buffer1)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, buffer0.fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buffer1.fbo); // Normal FBO can be the default FBO too.

	glBlitFramebuffer(0, 0, buffer0.size.x, buffer0.size.y, 0, 0, buffer1.size.x, buffer1.size.y, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

void Engine::genMipmaps(TextureObj tex)
{
	char levels = log2(128);

	glBindTexture(GL_TEXTURE_2D, tex.id);
	for (int level = 1; level > 8; level++)
	{
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, tex.id, level);
	}
}

glm::mat4 Engine::lookat(glm::vec3 pos, glm::vec3 lookatpos, float z_rotation)
{
	glm::vec3 lookvec = lookatpos - pos;
	float x_rotation = atan2(-lookvec.y, glm::length(glm::vec2(lookvec.x, lookvec.z)));
	float y_rotation = atan2( lookvec.x, -lookvec.z) ;


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
		fin = xrotation*yrotation*zrotation; // this works perfectly
	else
		fin = xrotation*yrotation; // if no z_rotation specified, don't bother multiplying

	return fin;
}

glm::mat4 Engine::translate(glm::vec3 pos)
{
	glm::mat4 temp = glm::mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		pos.x, pos.y, pos.z, 1.0);

	return temp;
}

glm::mat4 Engine::scale(glm::vec3 s)
{
	glm::mat4 temp = glm::mat4(
		s.x, 0.0, 0.0, 0.0,
		0.0, s.y, 0.0, 0.0,
		0.0, 0.0, s.z, 0.0,
		0.0, 0.0, 0.0, 1.0);

	return temp;
}

void Engine::buildSceneEntity(int index)
{
	if (sceneScript.entityArray[index].structureType == 0)
	{
		int meshInd;
		if (!sceneScript.entityArray[index].genPlane)
		{
			meshInd = meshManager.loadMesh(sceneScript.entityArray[index].meshName);
		}
		else
		{
			meshInd = meshManager.createPlane(sceneScript.entityArray[index].planeSize);
		}

		meshManager.meshArray[meshInd].collision = sceneScript.entityArray[index].collision;
		meshManager.meshArray[meshInd].visible = sceneScript.entityArray[index].visible;
		meshManager.meshArray[meshInd].culling = sceneScript.entityArray[index].culling;
		meshManager.meshArray[meshInd].transparent = sceneScript.entityArray[index].transparent;
		meshManager.meshArray[meshInd].castShadow = sceneScript.entityArray[index].castShadow;
		meshManager.meshArray[meshInd].lookatCam = sceneScript.entityArray[index].lookatCam;
		meshManager.meshArray[meshInd].lookatPos = sceneScript.entityArray[index].lookatPos;
		meshManager.meshArray[meshInd].lookatPosState = sceneScript.entityArray[index].lookatPosState;
		meshManager.meshArray[meshInd].follow = sceneScript.entityArray[index].follow;
		meshManager.meshArray[meshInd].reflectable = sceneScript.entityArray[index].reflectable;
		meshManager.meshArray[meshInd].isWater = sceneScript.entityArray[index].isWater;

		meshManager.meshArray[meshInd].pos = sceneScript.entityArray[index].pos;
		meshManager.meshArray[meshInd].scale = sceneScript.entityArray[index].scale;
		meshManager.meshArray[meshInd].rot = sceneScript.entityArray[index].rot;


		// assign textures
		for (int n = 0; n < 16; n++)
		{
			if (sceneScript.entityArray[index].texAssigned[n])
			{
				int exists = -1;
				// determine if texture already exists
				for (int t = 0; t < textureManager.texCount; t++)
				{
					if (strcmp(textureManager.texArray[t].name, sceneScript.entityArray[index].texName[n]) == 0)
						exists = t;
				}

				if (exists == -1)
				{
					printf("loading %s \n", sceneScript.entityArray[index].texName[n]);
					int a = textureManager.loadImage(sceneScript.entityArray[index].texName[n]);

					if (!sceneScript.entityArray[index].texWrap[n])
					{
						glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
						glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
						textureManager.texArray[a].wrapU = false;
						textureManager.texArray[a].wrapV = false;
					}
					else
					{
						glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
						glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
						textureManager.texArray[a].wrapU = true;
						textureManager.texArray[a].wrapV = true;
					}

					meshManager.meshArray[meshInd].texture[n] = textureManager.texArray[a].id;
					meshManager.meshArray[meshInd].texIndex[n] = a;
				}
				else
				{
					// if texture already exists, assign that to the mesh
					meshManager.meshArray[meshInd].texture[n] = textureManager.texArray[exists].id;
					meshManager.meshArray[meshInd].texIndex[n] = exists;
					//meshManager.setTexture(meshInd, exists, n);
				}
			}
		}

		// assign shader
		if (sceneScript.entityArray[index].shaderAssigned)
		{
			char* vert;
			char* frag;
			vert = sceneScript.entityArray[index].vertShader;
			frag = sceneScript.entityArray[index].fragShader;

			int exists = -1;
			for (int n = 0; n < shaderManager.shaderCount; n++)
			{
				if (strcmp(vert, shaderManager.shaderArray[n].vertexName) == 0)
					if (strcmp(frag, shaderManager.shaderArray[n].fragName) == 0)
						exists = n;
			}

			if (exists == -1)
			{
				int a = shaderManager.loadShader(vert, frag);
				meshManager.meshArray[meshInd].shaderIndex = a;
			}
			else
			{
				meshManager.meshArray[meshInd].shaderIndex = exists;
			}
		}

		// assign shader uniforms
		for (int n = 0; n < sceneScript.entityArray[index].uniformCount; n++)
		{
			char* uname = sceneScript.entityArray[index].uniformName[n];
			float v0 = sceneScript.entityArray[index].uniformVal[0][n];
			float v1 = sceneScript.entityArray[index].uniformVal[1][n];
			float v2 = sceneScript.entityArray[index].uniformVal[2][n];
			float v3 = sceneScript.entityArray[index].uniformVal[3][n];
			meshManager.setShaderConst(meshInd, uname, v0, v1, v2, v3);
		}

		meshManager.setShaderConst(meshInd, "samples", samples, 0, 0, 0);
		meshManager.setShaderConst(meshInd, "resolution", res.x, res.y, 0, 0);
		meshManager.setShaderConst(meshInd, "sundir", sundir.x, sundir.y, sundir.z, 0.f);
		
		if (meshManager.meshArray[meshInd].useVertexLight)
			meshManager.setShaderConst(meshInd, "GI", 1, 0, 0, 0);
		else
			meshManager.setShaderConst(meshInd, "GI", 0, 0, 0, 0);

		// assign shadow map
		meshManager.setTexture(meshInd, shadowMapComp.depth, 15);
		meshManager.setTexture(meshInd, shadowMapComp.color, 14);

		// assign depth map
		if (sceneScript.entityArray[index].depthMap)
		{
			if (samples > 1.0)
			{
				meshManager.setTexture(meshInd, renderImgMS[0].depth, 16);
			}
			
			meshManager.setTexture(meshInd, renderImg[0].depth, 10);
		}

		// assign opaque map
		if (sceneScript.entityArray[index].opaqueMap)
		{
			if (samples > 1.0)
			{
				meshManager.setTexture(meshInd, renderImg[0].color, 11);
			}
			else
			{
				meshManager.setTexture(meshInd, renderImg[1].color, 11);
			}
		}

		// assign reflection map
		if (sceneScript.entityArray[index].reflectMap)
		{

			meshManager.setTexture(meshInd, reflectionImg.color, 12);
		}

		// set mesh to follow camera
		if (meshManager.meshArray[meshInd].follow)
		{
			meshManager.meshArray[meshInd].followPos = meshManager.meshArray[meshInd].pos;
		}

		if (meshManager.meshArray[meshInd].lookatPosState)
		{
			glm::mat4 uu = meshManager.meshArray[meshInd].lookat(meshManager.meshArray[meshInd].lookatPos, 0.0f);
			meshManager.meshArray[meshInd].updateModelMatrix(uu);
		}
		else
		{
			meshManager.meshArray[meshInd].updateModelMatrix();
		}
		return;
	}

	// create particle system
	if (sceneScript.entityArray[index].structureType == 1)
	{
		int partInd;

		if (!sceneScript.entityArray[index].genPlane)
		{
			//partInd = particleManager.loadMesh(sceneScript.entityArray[index].meshName);
		}
		else
		{
			partInd = meshManager.createPlane(sceneScript.entityArray[index].planeSize);
		}

		meshManager.meshArray[partInd].visible = sceneScript.entityArray[index].visible;
		meshManager.meshArray[partInd].culling = sceneScript.entityArray[index].culling;
		meshManager.meshArray[partInd].transparent = sceneScript.entityArray[index].transparent;
		meshManager.meshArray[partInd].castShadow = sceneScript.entityArray[index].castShadow;
	}
}

glm::vec3 Engine::raycastPlane(glm::vec3 p0, glm::vec3 p1, glm::vec3 norm, glm::vec3 orig)
{
	glm::vec3 raydir = glm::normalize(p1 - p0);
	float raylength = glm::length(p1 - p0);
	
	float dotNormRaydir = glm::dot(raydir, norm);

	float dotNormPoint = glm::dot(p0-orig, norm);

	// continue only if ray can possibly intersect plane
	if ((dotNormRaydir > 0 && dotNormPoint < 0) || (dotNormRaydir < 0 && dotNormPoint > 0))
	{
		// return false if ray is coplanar with triangle
		if (abs(dotNormRaydir) < 0.0001) return glm::vec3(-1);

		// D component
		float planeOffset = glm::dot(norm, orig);
		float t = 0.0;

		t = (glm::dot(-p0, norm) + planeOffset) / (dotNormRaydir);

		// triangle out of range
		if (t > raylength) return glm::vec3(-1);
		// ray occurs behind triangle
		if (t < 0.0) return glm::vec3(-1);

		glm::vec3 intersectionPoint = p0 + raydir * t;

		return intersectionPoint;
	}
	else
		return glm::vec3(-1);
}

void Engine::sort2DItems()
{

	if (sorting2DCount != spriteManager.spriteCount + textManager.textCount)
	{
		delete[] sorting2DArray;
		sorting2DArray = new SortItem2D[spriteManager.spriteCount + textManager.textCount + 2];

		for (int n = 0; n < spriteManager.spriteCount; n++)
		{
			sorting2DArray[n].arrayIndex = n;
			sorting2DArray[n].depth = spriteManager.spriteArray[n].depth;
			sorting2DArray[n].type = SortItem2D::SPRITE;
		}
		for (int n = 0; n < textManager.textCount ; n++)
		{
			sorting2DArray[n + spriteManager.spriteCount].arrayIndex = n;
			sorting2DArray[n + spriteManager.spriteCount].depth = textManager.textArray[n].depth;
			sorting2DArray[n + spriteManager.spriteCount].type = SortItem2D::TEXT;
		}
		sorting2DCount = spriteManager.spriteCount + textManager.textCount;

		recQuickSort(0, sorting2DCount - 1);
	}
	
}

void Engine::recQuickSort(int left, int right)
{
	if (right - left <= 0)
		return;
	else
	{
		SortItem2D pivot = sorting2DArray[right];

		unsigned int partition = partitionIt(left, right, pivot);
		recQuickSort(left, partition - 1);
		recQuickSort(partition + 1, right);
	}
}

int Engine::partitionIt(int left, int right, SortItem2D pivot)
{
	int leftPtr = left - 1;
	int rightPtr = right;
	while (true)
	{
		while (sorting2DArray[++leftPtr].depth > pivot.depth)
			;
		while (rightPtr > 0 && sorting2DArray[--rightPtr].depth < pivot.depth)
			;
		if (leftPtr >= rightPtr)
			break;
		else
			swapItem2D(leftPtr, rightPtr);
	}
	swapItem2D(leftPtr, right);
	return leftPtr;
}

void Engine::swapItem2D(int dex1, int dex2)
{
	SortItem2D temp = sorting2DArray[dex1];
	sorting2DArray[dex1] = sorting2DArray[dex2];
	sorting2DArray[dex2] = temp;
}

void Engine::setMultisampleLevel(char lv)
{
	if (lv != samples)
	{
		
		// change multisample level
		if (lv != 0 && samples != 0)
		{
			glDeleteTextures(1, &renderImgMS[0].colorID);
			glDeleteTextures(1, &renderImgMS[0].depthID);
			glDeleteFramebuffers(1, &renderImgMS[0].fbo);

			//glDeleteTextures(1, &renderImgMS[1].color);
			//glDeleteTextures(1, &renderImgMS[1].depth);
			//glDeleteFramebuffers(1, &renderImgMS[1].fbo);

			renderImgMS[0] = textureManager.createRenderTextureMultisample(res.x, res.y, lv);

			samples = lv;
			for (int n = 0; n < meshManager.meshCount; n++)
			{
				meshManager.setShaderConst(n, "samples", samples, 0, 0, 0);
				meshManager.setShaderConst(n, "resolution", res.x, res.y, 0, 0);
				//meshManager.meshArray[n].texture[11] = renderImg[0].color;
				//meshManager.meshArray[n].texture[16] = renderImgMS[0].depth;
				meshManager.setTexture(n, renderImg[0].color, 11);
				meshManager.setTexture(n, renderImgMS[0].depth, 16);
			}
			textureStateID[16] = -1;
		}

		// change to no multisampling
		if (lv == 0 && samples != 0)
		{
			glDeleteTextures(1, &renderImgMS[0].colorID);
			glDeleteTextures(1, &renderImgMS[0].depthID);
			glDeleteFramebuffers(1, &renderImgMS[0].fbo);

			//glDeleteTextures(1, &renderImgMS[1].color);
			//glDeleteTextures(1, &renderImgMS[1].depth);
			//glDeleteFramebuffers(1, &renderImgMS[1].fbo);

			samples = lv;
			for (int n = 0; n < meshManager.meshCount; n++)
			{

				meshManager.setShaderConst(n, "samples", samples, 0, 0, 0);
				meshManager.setShaderConst(n, "resolution", res.x, res.y, 0, 0);
				//meshManager.meshArray[n].texture[11] =  renderImg[1].color ;
				//0meshManager.meshArray[n].texture[10] =  renderImg[0].depth;
				meshManager.setTexture(n, renderImg[1].color, 11);
				meshManager.setTexture(n, renderImg[0].depth, 10);

			}
			textureStateID[16] = -1;
		}

		// change to multisampling
		if (lv != 0 && samples == 0)
		{
			renderImgMS[0] = textureManager.createRenderTextureMultisample(res.x, res.y, lv);


			samples = lv;
			for (int n = 0; n < meshManager.meshCount; n++)
			{
				meshManager.setShaderConst(n, "samples", samples, 0, 0, 0);
				meshManager.setShaderConst(n, "resolution", res.x, res.y, 0, 0);
				//meshManager.meshArray[n].texture[11] = renderImg[0].color;
				//meshManager.meshArray[n].texture[16] = renderImgMS[0].depth;
				meshManager.setTexture(n, renderImg[0].color, 11);
				meshManager.setTexture(n, renderImgMS[0].depth, 16);
			}
			textureStateID[16] = -1;
		}

		
		//shaderManager.setMultisampleState(samples);
	}
}

void Engine::setResolution(int width, int height)
{
	if (int(res.x) == width && int(res.y) == height) return;

	// multisampling
	if (samples != 0)
	{
		glDeleteTextures(1, &renderImg[0].colorID);
		glDeleteTextures(1, &renderImg[0].depthID);
		glDeleteFramebuffers(1, &renderImg[0].fbo);

		glDeleteTextures(1, &renderImgMS[0].colorID);
		glDeleteTextures(1, &renderImgMS[0].depthID);
		glDeleteFramebuffers(1, &renderImgMS[0].fbo);

		renderImg[0] = textureManager.createRenderTexture(width, height, 0);
		renderImgMS[0] = textureManager.createRenderTextureMultisample(width, height, samples);
		res = glm::vec2(width, height);

		for (int n = 0; n < meshManager.meshCount; n++)
		{
			meshManager.setShaderConst(n, "samples", samples, 0, 0, 0);
			meshManager.setShaderConst(n, "resolution", res.x, res.y, 0, 0);
			meshManager.setTexture(n, renderImg[0].color, 11);
			meshManager.setTexture(n, renderImgMS[0].depth, 16);
		}

		// blur buffer
		glDeleteTextures(1, &convolutionBuffer[0].colorID);
		glDeleteTextures(1, &convolutionBuffer[0].depthID);
		glDeleteFramebuffers(1, &convolutionBuffer[0].fbo);
		convolutionBuffer[0] = textureManager.createRenderTexture(res.x, res.y, 0);
		

		// bloom
		glDeleteTextures(1, &bloomBuffer[0].colorID);
		glDeleteTextures(1, &bloomBuffer[0].depthID);
		glDeleteFramebuffers(1, &bloomBuffer[0].fbo);
		bloomBuffer[0] = textureManager.createRenderTexture(res.x / 2, res.y / 2, 0);
		glDeleteTextures(1, &bloomBuffer[1].colorID);
		glDeleteTextures(1, &bloomBuffer[1].depthID);
		glDeleteFramebuffers(1, &bloomBuffer[1].fbo);
		bloomBuffer[1] = textureManager.createRenderTexture(res.x / 4, res.y / 4, 0);

		// FXAA
		glDeleteTextures(1, &FXAA_lumaBuffer.colorID);
		glDeleteTextures(1, &FXAA_lumaBuffer.depthID);
		glDeleteFramebuffers(1, &FXAA_lumaBuffer.fbo);
		FXAA_lumaBuffer = textureManager.createRenderTexture(res.x, res.y, 0);

		// lightshafts
		
		glDeleteTextures(1, &lightShaftBuffer[0].colorID);
		glDeleteTextures(1, &lightShaftBuffer[0].depthID);
		glDeleteFramebuffers(1, &lightShaftBuffer[0].fbo);

		glDeleteTextures(1, &lightShaftBuffer[1].colorID);
		glDeleteTextures(1, &lightShaftBuffer[1].depthID);
		glDeleteFramebuffers(1, &lightShaftBuffer[1].fbo);

		glDeleteTextures(1, &lightShaftBuffer[2].colorID);
		glDeleteTextures(1, &lightShaftBuffer[2].depthID);
		glDeleteFramebuffers(1, &lightShaftBuffer[2].fbo);
		lightShaftBuffer[0] = textureManager.createRenderTexture(res.x, res.y, 0);
		lightShaftBuffer[1] = textureManager.createRenderTexture(res.x, res.y, 0);
		lightShaftBuffer[2] = textureManager.createRenderTexture(res.x / 2, res.y / 2, 0);
		
	}

	// no multisampling
	if (samples == 0)
	{
		glDeleteTextures(1, &renderImg[0].colorID);
		glDeleteTextures(1, &renderImg[0].depthID);
		glDeleteFramebuffers(1, &renderImg[0].fbo);

		renderImg[0] = textureManager.createRenderTexture(width, height,0);
		
		res = glm::vec2(width, height);
		for (int n = 0; n < meshManager.meshCount; n++)
		{
			meshManager.setShaderConst(n, "samples", samples, 0, 0, 0);
			meshManager.setShaderConst(n, "resolution", res.x, res.y, 0, 0);
			meshManager.setTexture(n, renderImg[0].color, 11);
			meshManager.setTexture(n, renderImg[0].depth, 10);
		}

		// blur buffer
		glDeleteTextures(1, &convolutionBuffer[0].colorID);
		glDeleteTextures(1, &convolutionBuffer[0].depthID);
		glDeleteFramebuffers(1, &convolutionBuffer[0].fbo);
		convolutionBuffer[0] = textureManager.createRenderTexture(res.x, res.y, 0);

		// bloom
		glDeleteTextures(1, &bloomBuffer[0].colorID);
		glDeleteTextures(1, &bloomBuffer[0].depthID);
		glDeleteFramebuffers(1, &bloomBuffer[0].fbo);
		bloomBuffer[0] = textureManager.createRenderTexture(res.x / 2, res.y / 2, 0);
		glDeleteTextures(1, &bloomBuffer[1].colorID);
		glDeleteTextures(1, &bloomBuffer[1].depthID);
		glDeleteFramebuffers(1, &bloomBuffer[1].fbo);
		bloomBuffer[1] = textureManager.createRenderTexture(res.x / 4, res.y / 4, 0);

		// FXAA
		glDeleteTextures(1, &FXAA_lumaBuffer.colorID);
		glDeleteTextures(1, &FXAA_lumaBuffer.depthID);
		glDeleteFramebuffers(1, &FXAA_lumaBuffer.fbo);
		FXAA_lumaBuffer = textureManager.createRenderTexture(res.x, res.y, 0);

		// lightshafts
		glDeleteTextures(1, &lightShaftBuffer[0].colorID);
		glDeleteTextures(1, &lightShaftBuffer[0].depthID);
		glDeleteFramebuffers(1, &lightShaftBuffer[0].fbo);

		glDeleteTextures(1, &lightShaftBuffer[1].colorID);
		glDeleteTextures(1, &lightShaftBuffer[1].depthID);
		glDeleteFramebuffers(1, &lightShaftBuffer[1].fbo);

		glDeleteTextures(1, &lightShaftBuffer[2].colorID);
		glDeleteTextures(1, &lightShaftBuffer[2].depthID);
		glDeleteFramebuffers(1, &lightShaftBuffer[2].fbo);
		lightShaftBuffer[0] = textureManager.createRenderTexture(res.x, res.y, 0);
		lightShaftBuffer[1] = textureManager.createRenderTexture(res.x, res.y, 0);
		lightShaftBuffer[2] = textureManager.createRenderTexture(res.x / 2, res.y / 2, 0);
	}


	meshManager.setTexture(renderQuad, renderImg[0].color, 0);
	meshManager.setShaderConst(FXAA_Quad, "resolution", res.x, res.y, 0, 0);
	for (unsigned char n = 0 ; n<17; n++)
		textureStateID[n] = -1;
	
}

void Engine::SampleVertexGI(MeshObj *mesh, bool doublesided)
{

	printf(mesh->name);
	printf("%d \n", mesh->indices.size);
	mesh->ind_GI = Stack<glm::vec3>(mesh->indices.size);
	printf("%d \n", mesh->ind_GI.maxSize);

	Octree<IrradianceSample> GItree(mesh->minpos + mesh->pos, mesh->maxpos + mesh->pos, 3);

	GLfloat r, g, b;

	GLfloat depthSample[5][4096];
	GLfloat colorSample[5][4096];
	float val0[3] = { 0.0f };
	float val1[3] = { 0.0f };

	float zNear = .1f;
	float zFar = 2500.0;
	float z_b = 1.0;
	float z_n = 2.0 * z_b - 1.0;
	float realdep0 = 2.0 * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));

	int samplesTaken = 0;
	glm::vec3 rotGrad[3];
	glm::vec3 transGrad[3];
	float sampleRotation;
	float mindepth = 0.f;
	glm::mat3 frame;

	int a = 1.0;
	bool flip;
	bool isFoliage;

	int GI_pass;
	std::cout << "GI pass";
	std::cin >> GI_pass;

	std::cout << "foliage?";
	std::cin >> isFoliage;
	if (!isFoliage)
	{
		std::cout << "gradient divisor";
		std::cin >> a;
	}
	for (int recordsSet = 0; recordsSet < 2; recordsSet++)
	{
		for (uint pp = 0; pp < mesh->indices.size; pp++)
		{

			int index = mesh->indices.item[pp];
			int vert = pp % 3;
			glm::vec3 tricenter;
			if (vert == 0)
			{
				int other0 = mesh->indices.item[pp + 1];
				int other1 = mesh->indices.item[pp + 2];
				tricenter = (mesh->ind_vertex.item[index] + mesh->ind_vertex.item[other0] + mesh->ind_vertex.item[other1]) / 3.0f;
			}
			if (vert == 1)
			{
				int other0 = mesh->indices.item[pp - 1];
				int other1 = mesh->indices.item[pp + 1];
				tricenter = (mesh->ind_vertex.item[other0] + mesh->ind_vertex.item[index] + mesh->ind_vertex.item[other1]) / 3.0f;
			}
			if (vert == 2)
			{
				int other0 = mesh->indices.item[pp - 2];
				int other1 = mesh->indices.item[pp - 1];
				tricenter = (mesh->ind_vertex.item[other0] + mesh->ind_vertex.item[other1] + mesh->ind_vertex.item[index]) / 3.0f;
			}

			glm::vec4 transvert = mesh->transformMatrix*glm::vec4(mesh->ind_vertex.item[index],1);
			glm::vec3 pos = glm::vec3(transvert) + mesh->pos;
			glm::vec3 norm = glm::mat3(mesh->transformMatrix) * mesh->ind_normal.item[index];
			//if (isFoliage) norm = glm::vec3(0, 1, 0);

			glm::vec3 offsetsamplepoint;
			if (glm::length(mesh->ind_vertex.item[index] - tricenter) > .15)
			{
				offsetsamplepoint = glm::normalize(mesh->ind_vertex.item[index] - tricenter)*glm::length(mesh->ind_vertex.item[index] - tricenter)*.15f;
				pos = mesh->ind_vertex.item[index] + mesh->pos - offsetsamplepoint;
				//
			//	printf("\n %f %f %f -> %f %f %f -> %f %f %f", tricenter.x, tricenter.y, tricenter.z , mesh->ind_vertex.item[index].x, mesh->ind_vertex.item[index].y, mesh->ind_vertex.item[index].z, offsetsamplepoint.x, offsetsamplepoint.y, offsetsamplepoint.z);
			}
			else
			{
				pos = tricenter + mesh->pos;
			}

			

			if (recordsSet == 1)
			{
				IrradianceSample *cacheSample = GItree.search(pos, norm);
				if (cacheSample != NULL)
				{
					mesh->ind_GI.item[index] = cacheSample->light;
					continue;
				}
			}
			if (recordsSet == 0)
			{
				if (GItree.scanRecords(pos, norm))
					continue;
			}

			GICamera.pos = pos;
			GICamera.forward = norm;// glm::vec3(0, 1, 0);

			frame = CameraManager::lookatMatrix(GICamera, pos + norm, glm::radians(0.f));

			int sides = 1;
			//if (doublesided || isFoliage) sides = 2;
			
			//two sided render twice
			for (int pass = 0; pass < sides; pass++)
			{
				if (pass == 1)
				{
					norm *= -1.0;
					//GICamera.forward = norm;
				}
				sampleRotation = 0;// rand() % 180;
				samplepoint(pos, norm, sampleRotation, isFoliage, GI_pass);

				for (int cubeFace = 0; cubeFace < 5; cubeFace++)
				{
					//glActiveTexture(GL_TEXTURE16);
					glBindTexture(GL_TEXTURE_2D, lightSampleGIBuffer[cubeFace].colorID);

					glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, &colorSample[cubeFace]);


					if (isFoliage) break; // only need to do forward sample
				}

				if (!isFoliage)
				{
					for (int cubeFace = 0; cubeFace < 5; cubeFace++)
					{
						glActiveTexture(GL_TEXTURE16);
						glBindTexture(GL_TEXTURE_2D, lightSampleGIBuffer[cubeFace].depthID);

						glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, &depthSample[cubeFace]);


					}
				}

				glm::dvec3 rot_grad_sum[3] = { glm::dvec3(0), glm::dvec3(0), glm::dvec3(0) };
				float areacount = 0;
				float halfsize = 16.f;
				float gradWeight = 0.0f;
				mindepth = 1.0;
				
				//particles.pos[samplesTaken] = pos;
				
				for (int cubeFace = 0; cubeFace <5; cubeFace++)
				{
					glm::vec3 texelDir;
					for (int j = 0; j < 32; j++)
					{
						
						for (int k = 0; k < 32; k++)
						{
							

							int index = 3 * (j + k*32 );
							r = colorSample[cubeFace][index];
							g = colorSample[cubeFace][index + 1];
							b = colorSample[cubeFace][index + 2];

							float x = (j - halfsize) / halfsize;
							float y = -(k - halfsize) / halfsize;

							
							if (cubeFace == 0) texelDir = glm::normalize(glm::vec3(x, y, 1)); // forward
							if (cubeFace == 1) texelDir = glm::normalize(glm::vec3(1, y, -x)); // right
							if (cubeFace == 2) texelDir = glm::normalize(glm::vec3(-1, y, x)); // left
							if (cubeFace == 3) texelDir = glm::normalize(glm::vec3(x, 1, y)); // up 
							if (cubeFace == 4) texelDir = glm::normalize(glm::vec3(x, -1, -y)); // down
							
							
							float angle = glm::dot(glm::vec3(0, 0, 1), texelDir);
							if (angle < 0.05) continue;

							
							if (pass == 0)
							{
								val0[0] += r * angle;
								val0[1] += g * angle;
								val0[2] += b * angle;
							}
							if (pass == 1)
							{
								val1[0] += r * angle;
								val1[1] += g * angle;
								val1[2] += b * angle;
							}

							areacount += angle;

							if (!isFoliage)
							{
								texelDir.z = glm::max(.05f, texelDir.z);

								if (abs(texelDir.x) < 0.05 && abs(texelDir.y) < 0.05) continue;
								glm::vec2 rotUnitVec = glm::normalize(glm::vec2(texelDir.x, texelDir.y));

								//float theta = atan2( glm::length(glm::vec2(texelDir.x, texelDir.y)), texelDir.z);

								glm::vec3 gradDir = glm::vec3(rotUnitVec.y / texelDir.z, -rotUnitVec.x / texelDir.z, 0);

								rot_grad_sum[0] += r * gradDir;
								rot_grad_sum[1] += g * gradDir;
								rot_grad_sum[2] += b * gradDir;

								gradWeight += 1.0f / texelDir.z;

								if (depthSample[cubeFace][(j + 32 * k)] > .01)
									if (mindepth > depthSample[cubeFace][(j + 32 * k)]) mindepth = depthSample[cubeFace][(j + 32 * k)];
							}
						}

					}
					
					if (isFoliage) break;
				}
				
				
				if (!isFoliage)
				{
					for (int i = 0; i < 3; i++)
					{
						rotGrad[i] = frame * rot_grad_sum[i];
						rotGrad[i].z *= -1;

						rotGrad[i] *= 1.0 / a;

					}
				}

				if (pass == 0)
				{
					val0[0] /= areacount;
					val0[1] /= areacount;
					val0[2] /= areacount;

				}
				if (pass == 1)
				{
					val1[0] /= areacount;
					val1[1] /= areacount;
					val1[2] /= areacount;
				}

			}
			
			if (doublesided || isFoliage)
				mesh->ind_GI.item[index] = (glm::vec3(val0[0], val0[1], val0[2]) + glm::vec3(val1[0], val1[1], val1[2]));
			else
				mesh->ind_GI.item[index] = glm::vec3(val0[0], val0[1], val0[2]);


			float realdep0;
			if (!isFoliage)
			{
				float z_b = mindepth;
				float z_n = 2.0f * z_b - 1.0f;
				realdep0 = 2.0f * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));
			}

			if (!isFoliage) 
				GItree.add(IrradianceSample(mesh->ind_GI.item[index], glm::min(4.f*realdep0+.5f, 30.0f), pos, norm, rotGrad));
			else
				GItree.add(IrradianceSample(mesh->ind_GI.item[index], .5f, pos, norm, rotGrad));

			samplesTaken++;
		}

		//glBindBuffer(GL_ARRAY_BUFFER, particles.posBufferID);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*particles.count, particles.pos, GL_STREAM_DRAW);
	}
	mesh->ind_GI.size = mesh->ind_vertex.size;

	printf("\n Done %d\n", samplesTaken);
	//Shader *shader = &shaderManager.shaderArray[mesh->shaderIndex];

	//mesh->setShaderConst(shader, "GI", 1, 0, 0, 0);
	for (int n = 0; n < meshManager.meshCount; n++)
		if (meshManager.meshArray[n].useVertexLight) meshManager.setShaderConst(n, "GI", 1, 0, 0, 0);

	//gi buffer
	glGenBuffers(1, &mesh->vertGIBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertGIBuffer);
	glBufferData(GL_ARRAY_BUFFER, mesh->ind_GI.size * sizeof(glm::vec3), &mesh->ind_GI.item[0], GL_STATIC_DRAW);

}

// Solid angle of an axis aligned quad from (0,0,1) to (x,y,1)
// See: http://www.fizzmoll11.com/thesis/ for a derivation of this formula.
float Engine::area_element(float x, float y)
{
	return atan2(x*y, sqrtf(x*x + y * y + 1));
}

// Solid angle of a hemicube texel.
float Engine::solid_angle_term(const glm::vec2 & f, int cube_size) 
{
#if 1   // Exact solid angle:
	float x0 = f.x - (1.0f / cube_size);
	float y0 = f.y - (1.0f / cube_size);
	float x1 = f.x + (1.0f / cube_size);
	float y1 = f.y + (1.0f / cube_size);
	float solid_angle = area_element(x0, y0) - area_element(x0, y1) - area_element(x1, y0) + area_element(x1, y1);
	assert(solid_angle > 0.0f);

	return solid_angle;

#else   // This formula is equivalent, but not as precise.
	float pixel_area = square(2.0f / cube_size);
	float dist_square = 1.0f + square(f.x) + square(f.y);
	float cos_theta = 1.0f / sqrt(dist_square);
	float cos_theta_d2 = cos_theta / dist_square; // Funny this is just 1/dist^3 or cos(tetha)^3
	return pixel_area * cos_theta_d2;
#endif
}

float Engine::clamped_cosine_term(const glm::vec3 & dir) 
{
	return glm::clamp(dir.z, 0.0f, 1.0f);
}

bool Engine::pointInTriangleUV(glm::vec2 uv0, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 point)
{
	//return true;
	if (point == glm::floor(uv0)) return true;
	if (point == glm::floor(uv1)) return true;
	if (point == glm::floor(uv2)) return true;


	glm::vec2 v0 = glm::normalize((uv1 - uv0));
	glm::vec2 v1 = glm::normalize((uv2 - uv0));
	glm::vec2 v2 = glm::normalize((point - uv0));

	float a = -glm::dot(v0, v1);
	float b = -glm::dot(v2, v0);

	if (a >= b)
	{
		float c = -glm::dot(v2, v1);

		if (a >= c)
		{

			glm::vec2 v3 = glm::normalize(glm::floor(uv0 - uv1));
			glm::vec2 v4 = glm::normalize(glm::floor(uv2 - uv1));
			glm::vec2 v5 = glm::normalize(glm::floor(point - uv1));

			a = -glm::dot(v3, v4);
			b = -glm::dot(v5, v3);
			//c = -glm::dot(v5, v4);

			if (a >= b ) return true;
		}
	}
	return false;
}

glm::vec3 Engine::interpolateTriangle(glm::vec3 vert0, glm::vec3 vert1, glm::vec3 vert2, glm::vec3 point)
{
	glm::vec3 f1 = vert0 - point;
	glm::vec3 f2 = vert1 - point;
	glm::vec3 f3 = vert2 - point;

	float a = glm::length(glm::cross(vert0 - vert1, vert0 - vert2));
	float a1 = glm::length(glm::cross(f2, f3)) / a;
	float a2 = glm::length(glm::cross(f3, f1)) / a;
	float a3 = glm::length(glm::cross(f1, f2)) / a;

	return glm::vec3(a1, a2, a3);
}

glm::vec3 Engine::interpolateTriangleUV(glm::vec2 uv0, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 point)
{
	glm::vec3 f1 = glm::vec3(uv0 - point, 0);
	glm::vec3 f2 = glm::vec3(uv1 - point, 0);
	glm::vec3 f3 = glm::vec3(uv2 - point, 0);

	float area = glm::length( glm::cross( glm::vec3(uv0 - uv1, 0), glm::vec3(uv0 - uv2, 0)) );
	float a1 = glm::length(glm::cross(f2, f3)) / area;
	float a2 = glm::length(glm::cross(f3, f1)) / area;
	float a3 = glm::length(glm::cross(f1, f2)) / area;

	return glm::vec3(a1, a2, a3);
}

// margin of error
bool Engine::moe(float input, float check, float margin)
{
	if ((input + margin >= check) || (input - margin <= check)) return true;
	return false;
}

void Engine::samplepoint(glm::vec3 pos, glm::vec3 norm, float rotation, bool foliage, char GI_pass)
{
	GICamera.pos = pos;
	GICamera.forward = norm;// glm::vec3(0, 1, 0);

	generateShadowMapGI();

	//render opaque geometry
	viewMatrix = CameraManager::lookatMatrix(GICamera, pos + norm, glm::radians(rotation)); //glm::radians(1.0f*(rand() % 180))
	if (!foliage) 
		projectionMatrix = glm::perspective(3.14159 / 2.0, 1.0, .1, 2500.0);
	else
		projectionMatrix = glm::perspective(3.14159 / 2.0, 1.0, .1, 2500.0);

	view_projMatrix = projectionMatrix * viewMatrix;
	
	
	if (!foliage)
	{

		newView[0] = look_right * viewMatrix;
		newView[1] = look_left * viewMatrix;
		newView[2] = look_up * viewMatrix;
		newView[3] = look_down * viewMatrix;

		newViewProj[0] = projectionMatrix * newView[0];
		newViewProj[1] = projectionMatrix * newView[1];
		newViewProj[2] = projectionMatrix * newView[2];
		newViewProj[3] = projectionMatrix * newView[3];
	}


	for (int cubeFace = 0; cubeFace < 5; cubeFace++)
	{

		if (cubeFace > 0) view_projMatrix = newViewProj[cubeFace - 1];
		setRenderToTexture(lightSampleGIBuffer[cubeFace], 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw opaque geometry
		for (int n = 0; n < meshManager.meshCount; n++)
		{

			if (meshManager.meshArray[n].type == STANDARD)
			{

				if (!meshManager.meshArray[n].isWater)
				{
					if (!meshManager.meshArray[n].transparent)
					{
						meshManager.setShaderConst(n, "campos", GICamera.pos.x, GICamera.pos.y, GICamera.pos.z, 0.f);
						meshManager.setShaderConst(n, "shadowMapSize", 256, 256, 0, 0);
						meshManager.setShaderConst(n, "GI", GI_pass, 0, 0, 1);
						meshManager.setShaderConst(n, "reflectHeight", -5000, 0, 0, 0);
						meshManager.setShaderConst(n, "cascadeCenter0", cascadeCenter[0].x, cascadeCenter[0].y, cascadeCenter[0].z, 0.f);
						meshManager.setShaderConst(n, "cascadeCenter1", cascadeCenter[1].x, cascadeCenter[1].y, cascadeCenter[1].z, 0.f);
						meshManager.setShaderConst(n, "cascadeCenter2", cascadeCenter[2].x, cascadeCenter[2].y, cascadeCenter[2].z, 0.f);
						meshManager.setShaderConst(n, "cascadeCenter3", cascadeCenter[3].x, cascadeCenter[3].y, cascadeCenter[3].z, 0.f);
						meshManager.setTexture(n, GI_shadowMapComp.depth, 15);
						drawMeshGI(&meshManager.meshArray[n]);
						meshManager.setShaderConst(n, "GI", 0, 0, 0, 0);
						meshManager.setTexture(n, shadowMapComp.depth, 15);
					}
				}

			}
		}
		if (foliage) break;
	}

}

void Engine::recompileShaders()
{
	for (int n = 0; n < shaderManager.shaderCount; n++)
	{
		int holdID = shaderManager.shaderArray[n].id;

		char *vert = shaderManager.shaderArray[n].vertexName;
		char *frag = shaderManager.shaderArray[n].fragName;

		shaderManager.remove(n);

		int newIndex = shaderManager.loadShader(vert, frag);

		for (int m = 0; m < meshManager.meshCount; m++)
		{
			if (meshManager.meshArray[m].shaderIndex == n)
			{

				meshManager.meshArray[m].shaderIndex = newIndex;
			}
		}
	}
	return;
}

void Engine::generateShadowMap()
{

	// render first shadow cascade
	setRenderToTexture(shadowMap[0], 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Camera matrix
	cascadeRadius[0] = 10; //10
	cascadeCenterDist[0] = 9.5; //9.5
	glm::vec3 center = mainCamera.pos + mainCamera.forward*cascadeCenterDist[0];

	shadowCamView[0] = lookat(center + 300.f*sundir, center, 0);
	glm::vec3 right = glm::vec3(shadowCamView[0][0][0], shadowCamView[0][1][0], shadowCamView[0][2][0]);
	glm::vec3 up = glm::vec3(shadowCamView[0][0][1], shadowCamView[0][1][1], shadowCamView[0][2][1]);

	float shadowmapFixSize = 32;

	float x = ceilf(glm::dot(center, up) * shadowmapFixSize / cascadeRadius[0]) *(cascadeRadius[0] / shadowmapFixSize);
	float y = ceilf(glm::dot(center, right) * shadowmapFixSize / cascadeRadius[0]) *(cascadeRadius[0] / shadowmapFixSize);
	center = up * x + right * y + sundir * glm::dot(center, sundir);
	glm::vec3 origin = center - sundir * 300.f;

	cascadeCenter[0] = center;

	shadowCamView[0] = glm::lookAtRH(origin, center, up);
	//shadowCamView[0] = lookat(cascadeCenter[0]+glm::vec3(100), cascadeCenter[0], 0)*translate(-cascadeCenter[0] -glm::vec3(100)); ;
	shadowOrthoProj[0] = glm::ortho(-cascadeRadius[0], cascadeRadius[0], -cascadeRadius[0], cascadeRadius[0], 1.0f, 500.0f);
	shadowViewProj[0] = shadowOrthoProj[0] * shadowCamView[0];

	for (int n = 0; n < meshManager.meshCount; n++)
	{
		if (!meshManager.meshArray[n].transparent && meshManager.meshArray[n].castShadow)
			drawMeshShadowMap(&meshManager.meshArray[n], 0);
	}

	// render second shadow cascade
	setRenderToTexture(shadowMap[1], 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	cascadeRadius[1] = 50.f;
	cascadeCenterDist[1] = 50;
	center = mainCamera.pos + mainCamera.forward*cascadeCenterDist[1];

	shadowCamView[1] = lookat(center + sundir*300.f, center, 0);
	right = glm::vec3(shadowCamView[1][0][0], shadowCamView[1][1][0], shadowCamView[1][2][0]);
	up = glm::vec3(shadowCamView[1][0][1], shadowCamView[1][1][1], shadowCamView[1][2][1]);

	x = ceilf(glm::dot(center, up) * shadowmapFixSize / cascadeRadius[1]) *(cascadeRadius[1] / shadowmapFixSize);
	y = ceilf(glm::dot(center, right) * shadowmapFixSize / cascadeRadius[1]) *(cascadeRadius[1] / shadowmapFixSize);
	center = up * x + right * y + sundir * glm::dot(center, sundir);
	origin = center - sundir * 300.f;


	cascadeCenter[1] = center;
	shadowCamView[1] = glm::lookAtRH(origin, center, up);
	shadowOrthoProj[1] = glm::ortho(-cascadeRadius[1], cascadeRadius[1], -cascadeRadius[1], cascadeRadius[1], 1.0f, 500.0f);
	shadowViewProj[1] = shadowOrthoProj[1] * shadowCamView[1];

	for (int n = 0; n < meshManager.meshCount; n++)
	{
		if (!meshManager.meshArray[n].transparent && meshManager.meshArray[n].castShadow)
			drawMeshShadowMap(&meshManager.meshArray[n], 1);
	}

	// render third shadow cascade
	setRenderToTexture(shadowMap[2], 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cascadeRadius[2] = 220.0f;
	cascadeCenterDist[2] = 190;
	center = mainCamera.pos + mainCamera.forward*cascadeCenterDist[2];

	shadowCamView[1] = lookat(center + sundir * 500.f, center, 0);
	right = glm::vec3(shadowCamView[1][0][0], shadowCamView[1][1][0], shadowCamView[1][2][0]);
	up = glm::vec3(shadowCamView[1][0][1], shadowCamView[1][1][1], shadowCamView[1][2][1]);

	x = ceilf(glm::dot(center, up) * shadowmapFixSize / cascadeRadius[2]) *(cascadeRadius[2] / shadowmapFixSize);
	y = ceilf(glm::dot(center, right) * shadowmapFixSize / cascadeRadius[2]) *(cascadeRadius[2] / shadowmapFixSize);
	center = up * x + right * y + sundir * glm::dot(center, sundir);
	origin = center - sundir * 500.f;

	cascadeCenter[2] = center;
	shadowCamView[2] = glm::lookAtRH(origin, center, up);
	shadowOrthoProj[2] = glm::ortho(-cascadeRadius[2], cascadeRadius[2], -cascadeRadius[2], cascadeRadius[2], 1.0f, 1000.f);
	shadowViewProj[2] = shadowOrthoProj[2] * shadowCamView[2];

	for (int n = 0; n < meshManager.meshCount; n++)
	{
		if (!meshManager.meshArray[n].transparent && meshManager.meshArray[n].castShadow)
			drawMeshShadowMap(&meshManager.meshArray[n], 2);
	}

	// render fourth shadow cascade
	setRenderToTexture(shadowMap[3], 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cascadeRadius[3] = 450;
	cascadeCenterDist[3] = 450;
	center = mainCamera.pos + mainCamera.forward*cascadeCenterDist[3];

	shadowCamView[1] = lookat(center + sundir*500.f, center, 0);
	right = glm::vec3(shadowCamView[1][0][0], shadowCamView[1][1][0], shadowCamView[1][2][0]);
	up = glm::vec3(shadowCamView[1][0][1], shadowCamView[1][1][1], shadowCamView[1][2][1]);

	x = ceilf(glm::dot(center, up) * shadowmapFixSize / cascadeRadius[3]) *(cascadeRadius[3] / shadowmapFixSize);
	y = ceilf(glm::dot(center, right) * shadowmapFixSize / cascadeRadius[3]) *(cascadeRadius[3] / shadowmapFixSize);
	center = up * x + right * y + sundir * glm::dot(center, sundir);
	origin = center - sundir * 500.f;

	cascadeCenter[3] = center;
	shadowCamView[3] = glm::lookAtRH(origin, center, up);
	shadowOrthoProj[3] = glm::ortho(-cascadeRadius[3], cascadeRadius[3], -cascadeRadius[3], cascadeRadius[3], 1.0f, 1000.0f);
	shadowViewProj[3] = shadowOrthoProj[3] * shadowCamView[3];

	for (int n = 0; n < meshManager.meshCount; n++)
	{
		if (!meshManager.meshArray[n].transparent && meshManager.meshArray[n].castShadow)
			drawMeshShadowMap(&meshManager.meshArray[n], 3);
	}

	//compile shadow maps into single buffer
	setRenderToTexture(shadowMapComp, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	drawMesh(&meshManager.meshArray[shadowCompQuad]);

	glBindTexture(GL_TEXTURE_2D, shadowMapComp.depthID);
	glGenerateMipmap(GL_TEXTURE_2D);

}

void Engine::generateShadowMapGI()
{
	// render first shadow cascade
	setRenderToTexture(GI_shadowMap[0], 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Camera matrix
	cascadeRadius[0] = 10; //10
	cascadeCenterDist[0] = 9.5; //9.5
	glm::vec3 center = GICamera.pos + GICamera.forward*cascadeCenterDist[0];

	shadowCamView[0] = lookat(center + glm::vec3(300), center, 0);
	glm::vec3 right = glm::vec3(shadowCamView[0][0][0], shadowCamView[0][1][0], shadowCamView[0][2][0]);
	glm::vec3 up = glm::vec3(shadowCamView[0][0][1], shadowCamView[0][1][1], shadowCamView[0][2][1]);
	glm::vec3 direction = glm::normalize(-glm::vec3(300));
	float shadowmapFixSize = 32;

	glm::vec3 origin = center - direction * 300.f;

	cascadeCenter[0] = center;
	shadowCamView[0] = glm::lookAtRH(origin, center, up);
	//shadowCamView[0] = lookat(cascadeCenter[0] + glm::vec3(100), cascadeCenter[0], 0)*translate(-cascadeCenter[0] -glm::vec3(100)); ;
	shadowOrthoProj[0] = glm::ortho(-cascadeRadius[0], cascadeRadius[0], -cascadeRadius[0], cascadeRadius[0], 1.0f, 500.0f);
	shadowViewProj[0] = shadowOrthoProj[0] * shadowCamView[0];

	for (int n = 0; n < meshManager.meshCount; n++)
	{
		if (!meshManager.meshArray[n].transparent && meshManager.meshArray[n].castShadow)
			drawMeshShadowMap(&meshManager.meshArray[n], 0);
	}

	// render second shadow cascade
	setRenderToTexture(GI_shadowMap[1], 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	cascadeRadius[1] = 50.f;
	cascadeCenterDist[1] = 50;
	center = GICamera.pos + GICamera.forward*cascadeCenterDist[1];

	shadowCamView[1] = lookat(center + glm::vec3(300), center, 0);
	right = glm::vec3(shadowCamView[1][0][0], shadowCamView[1][1][0], shadowCamView[1][2][0]);
	up = glm::vec3(shadowCamView[1][0][1], shadowCamView[1][1][1], shadowCamView[1][2][1]);
	direction = glm::normalize(-glm::vec3(300));

	origin = center - direction * 300.f;


	cascadeCenter[1] = center;
	shadowCamView[1] = glm::lookAtRH(origin, center, up);
	shadowOrthoProj[1] = glm::ortho(-cascadeRadius[1], cascadeRadius[1], -cascadeRadius[1], cascadeRadius[1], 1.0f, 500.0f);
	shadowViewProj[1] = shadowOrthoProj[1] * shadowCamView[1];

	for (int n = 0; n < meshManager.meshCount; n++)
	{
		if (!meshManager.meshArray[n].transparent && meshManager.meshArray[n].castShadow)
			drawMeshShadowMap(&meshManager.meshArray[n], 1);
	}

	// render third shadow cascade
	setRenderToTexture(GI_shadowMap[2], 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cascadeRadius[2] = 200.0f;
	cascadeCenterDist[2] = 190;
	center = GICamera.pos + GICamera.forward*cascadeCenterDist[2];

	shadowCamView[1] = lookat(center + glm::vec3(500), center, 0);
	right = glm::vec3(shadowCamView[1][0][0], shadowCamView[1][1][0], shadowCamView[1][2][0]);
	up = glm::vec3(shadowCamView[1][0][1], shadowCamView[1][1][1], shadowCamView[1][2][1]);
	direction = glm::normalize(-glm::vec3(500));

	origin = center - direction * 300.f;

	cascadeCenter[2] = center;
	shadowCamView[2] = glm::lookAtRH(origin, center, up);
	shadowOrthoProj[2] = glm::ortho(-cascadeRadius[2], cascadeRadius[2], -cascadeRadius[2], cascadeRadius[2], 1.0f, 800.f);
	shadowViewProj[2] = shadowOrthoProj[2] * shadowCamView[2];

	for (int n = 0; n < meshManager.meshCount; n++)
	{
		if (!meshManager.meshArray[n].transparent && meshManager.meshArray[n].castShadow)
			drawMeshShadowMap(&meshManager.meshArray[n], 2);
	}

	//compile shadow maps into single buffer
	setRenderToTexture(GI_shadowMapComp, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	drawMesh(&meshManager.meshArray[GI_shadowCompQuad]);


}