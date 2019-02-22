#ifndef CONTROL_H_
#define CONTROL_H_

#include "glm.hpp"
#include <glfw3.h>
#include "Camera.h"

#include <dinput.h>

struct KeyState
{
	KeyState()
	{
		down = false;
		downtimer = 0;
	}
	KeyState( const KeyState &other)
	{
		down = other.down;
		downtimer = other.downtimer;
	}

	bool down;
	int downtimer;
};

struct Controls
{
	IDirectInput *dinput;
	Controls::Controls();

	void typeInput();
	char typeIn[256];
	int typeInText;
	unsigned short keystateTimer[350];

	void Init(GLFWwindow *wnd);

	bool hoverRegion(glm::vec2 p0, glm::vec2 p1);
	bool clickRegion(glm::vec2 p0, glm::vec2 p1);
	bool Controls::grabRegion(glm::vec2 p0, glm::vec2 p1);
	bool clickOutsideRegion(glm::vec2 p0, glm::vec2 p1);

	double frameRate;
	GLFWwindow *window;

	glm::dvec3 position;
	glm::dvec3 last_position;
	float horizontalAngle;
	float verticalAngle;
	float initialFoV;

	double friction;
	double acceleration;
	glm::dvec3 velocity;
	float topspeed;
	float topspeed_walk;
	float topspeed_run;
	float speed;
	float mouseSpeed;

	glm::vec3 direction;
	glm::vec3 right;
	glm::vec3 up;

	glm::vec2 mouseHistory[10];

	void update();
	glm::dvec2 lastCursorPos;
	glm::dvec2 cursorPos;
	glm::dvec2 editorCursor;

	KeyState keystate[350];
	void updateKeystate(unsigned short key);

	glm::ivec2 windowSize;
	//void window_size_callback(GLFWwindow *window, int width, int height);
	bool windowResized;

	Camera activeCamera;
	bool run;

	bool mouse_leftDown;
	glm::vec2 mouseDownPos;
	glm::vec2 mouseUpPos;
	int downTimer;

	void resetMouseInput();
	bool grabbingState;

	enum
	{
		CAM_WALK,
		CAM_FLY
	};
	unsigned char cameraMode;

	int editorMode;
	double deltaWheel;
	double cummulativeWheel;

	bool lockCam;
};


#endif
