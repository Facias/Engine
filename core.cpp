#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#include <WinSock2.h>
#include <Windows.h>
#include <tchar.h>  



#ifndef GL_STUFF
#define GL_STUFF
	#include <glew.h>
	#include <glfw3.h>
	#include <wglew.h>
#endif

#include <iostream>
#include <fstream>
#include <vector>


#include "gtc/matrix_transform.hpp"
#include "gtx/transform.hpp"

#include "controls.h"
#include "EngineMain.h"

#include "Shader.h"
#include "Texture.h"
#include "Text.h"


 
int main()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		
		return -1;
	}

	


	glfwWindowHint(GLFW_SAMPLES, 8); // backbuffer ms antialiasing only
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 
	
	int monitorCount = 0;
	GLFWmonitor **monitor = glfwGetMonitors(&monitorCount);
				
	

	// Open a window and create its OpenGL context
	GLFWwindow* window; // (In the accompanying source code, this variable is global for simplicity)
	window = glfwCreateWindow(WIDTH, HEIGHT, "Engine", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		
		Sleep(10000);
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window); // Initialize GLEW
	glewExperimental = false; // Needed in core profile
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		Sleep(10000);
		return -1;
	}
	
	
	const GLubyte *tt;
	tt = glGetString(GL_VERSION);
	printf("%s\n", tt);
	glfwSetWindowPos(window, 300, 150);

	/*
	int monitorPosX, monitorPosY;
	glfwGetMonitorPos(monitor[0], &monitorPosX, &monitorPosY);
	int monitorSizeX, monitorSizeY;
	
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	float window_width = mode->width;
	float window_height = mode->height;


	glfwSetWindowMonitor(window, monitor[0], monitorPosX, monitorPosY, window_width, window_height, 60);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	

	glViewport(monitorPosX, monitorPosY, window_width, window_height);
	*/

	glEnable(GL_ARB_sample_shading);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_MULTISAMPLES_NV);
	glDisable(GL_STENCIL_TEST);
	//glfwWindowHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_DONT_CARE);

	// polygon smoothing seems to be causing render gaps between triangles?
	glDisable(GL_POLYGON_SMOOTH);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);

	ENGINE::Engine engine;
	engine.windowSize = glm::ivec2(WIDTH, HEIGHT);

	
	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);

	glClearColor(0, 0, 0, 0);

	engine.window = window;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	engine.init();


	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	wglSwapIntervalEXT(false);
	


	do
	{
		//Sleep(1.667);
		engine.updateFrameTimeInfo();
		//if (engine.getFrameTime() > .01) printf("%f\n", engine.getFrameTime());
		engine.mainLoop();

		

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	return 0;
}

