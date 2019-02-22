#include "controls.h"
#include <string>
#include <iostream>

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
double deltaScroll;

#undef min
#undef max

Controls::Controls()
{
	// position
	position = glm::vec3(0, 0, 5);
	// horizontal angle : toward -Z
	horizontalAngle = 3.14f;
	// vertical angle : 0, look at the horizon
	verticalAngle = 0.0f;
	// Initial Field of View
	initialFoV = 45.0f;

	acceleration = .15;
	friction = .25;
	topspeed = .125;
	topspeed_walk = .125;
	topspeed_run = 2.3;
	speed = .0f; // 3 units / second
	mouseSpeed = 0.5f;

	direction = glm::vec3(0, 0, 1);
	right = glm::vec3(1, 0, 0);
	up = glm::vec3(0, 1, 0);


	windowResized = false;
	editorMode = 0;
	frameRate = 1.0;
	window = NULL;

	deltaWheel = 0.0;
	cummulativeWheel = 0.0;


	strcpy(typeIn, "");
}

void windowResize(GLFWwindow *window)
{

}

void Controls::resetMouseInput()
{
	mouse_leftDown = false;
	mouseDownPos = glm::vec2(-2);
	mouseUpPos = glm::vec2(-1);
	downTimer = 0;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	deltaScroll = yoffset;

}

void Controls::update()
{

	if (!editorMode)
	{
		lastCursorPos = cursorPos;
		glfwGetCursorPos(window, &cursorPos.x, &cursorPos.y);
		
	}
	else
	{

		if (abs(deltaWheel - deltaScroll) < .1)
		{
			deltaScroll = 0.0;
		}
		glfwSetScrollCallback(window, scroll_callback);


		if (deltaWheel < deltaScroll -.01)
			deltaWheel += .1;
		if (deltaWheel > deltaScroll +.01)
			deltaWheel -= .1;
		activeCamera.FOV -= 1*deltaScroll;
		if (activeCamera.FOV > 120.0) activeCamera.FOV = 120.0;
		if (activeCamera.FOV < 60.0) activeCamera.FOV = 60.0;
		

		glfwGetWindowSize(window, &windowSize.x, &windowSize.y);
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_3))
		{
			if (lastCursorPos.x >= 0)lastCursorPos = cursorPos;
			glfwGetCursorPos(window, &cursorPos.x, &cursorPos.y);
			if (lastCursorPos.x <0) lastCursorPos = cursorPos;
		}
		else
		{
			glfwGetCursorPos(window, &cursorPos.x, &cursorPos.y);
			lastCursorPos = cursorPos;

			//lastCursorPos = glm::vec2(-1);
			//cursorPos = glm::vec2(-1);
		}

		glfwGetCursorPos(window, &editorCursor.x, &editorCursor.y);
	}

	// track mouse inputs while in editor mode
	if (editorMode)
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
		{
			if (mouse_leftDown)
			{
				downTimer++;
			}
			if (!mouse_leftDown)
			{
				downTimer = 0;
				mouse_leftDown = true;
				mouseDownPos = editorCursor;
			}
		}
		else
		{
			if (mouse_leftDown)
			{
				mouseUpPos = editorCursor;
				mouse_leftDown = false;
			}
		}
	}

	updateKeystate(GLFW_KEY_TAB);

	KeyState state = keystate[GLFW_KEY_TAB];
	switch (editorMode)
	{
	case 0:
		if (state.down && state.downtimer == 0)
		{
			editorMode = 1;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetWindowTitle(window, "Katina - Editor");
		}
		break;
	case 1:
		if (state.down && state.downtimer == 0)
		{
			editorMode = 0;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			
			glfwGetCursorPos(window, &cursorPos.x, &cursorPos.y);
			lastCursorPos = cursorPos;
			glfwSetWindowTitle(window, "Katina");
		}
		break;
	}

	double speedAdj = 60.0 / frameRate;


	if (!editorMode || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	{
		for (int n = 9; n > 0; n--)
			mouseHistory[n] = mouseHistory[n - 1];
		mouseHistory[0] = cursorPos - lastCursorPos;
	}
	else
	{
		for (int n = 9; n > 0; n--)
			mouseHistory[n] = mouseHistory[n - 1];
		mouseHistory[0] = glm::vec2(0);
	}

	//blend mouse temporal inputs
	glm::vec2 blendMouse = glm::vec2(0.0);
	blendMouse += mouseHistory[0] * .16105f;
	blendMouse += mouseHistory[1] * .158969f;
	blendMouse += mouseHistory[2] * .152766f;
	blendMouse += mouseHistory[3] * .142607f;
	blendMouse += mouseHistory[4] * .128754f;
	blendMouse += mouseHistory[5] * .111567f;
	blendMouse += mouseHistory[6] * .09149f;
	blendMouse += mouseHistory[7] * .069045f;
	blendMouse += mouseHistory[8] * .044814f;
	blendMouse += mouseHistory[9] * .019462f;


	// Compute new orientation
	if (!editorMode)
	{
		
		activeCamera.rot.y += mouseSpeed  * -(float(blendMouse.x) / 270.0f);
		activeCamera.rot.x += mouseSpeed  * -(float(blendMouse.y) / 270.0f);

	}
	else
	{
		if (!lockCam && !grabbingState)
		{
			activeCamera.rot.y += mouseSpeed  * -(float(blendMouse.x) / 50.f);
			activeCamera.rot.x += mouseSpeed  * -(float(blendMouse.y) / 50.f);
		}
	}
	activeCamera.rot.x = glm::min(glm::radians(89.0f), glm::max(-glm::radians(89.0f), activeCamera.rot.x));

	// Direction forward vector
	direction = glm::vec3(
		cos(activeCamera.rot.x) * sin(activeCamera.rot.y),
		sin(activeCamera.rot.x),
		cos(activeCamera.rot.x) * cos(activeCamera.rot.y)
		);

	// Right vector
	right = glm::vec3(
		sin(activeCamera.rot.y - 3.14f / 2.0f),
		0,
		cos(activeCamera.rot.y - 3.14f / 2.0f)
		);

	// Up vector : perpendicular to both direction and right
	up = glm::cross(right, direction);


	activeCamera.forward = direction;
	if ((editorMode && !glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) || !editorMode)
	{

		// Move forward
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			velocity += acceleration*glm::dvec3(direction)*speedAdj;
		}

		// Move backward
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			velocity -= acceleration * glm::dvec3(direction)*speedAdj;
		}

		// Strafe right
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			velocity += acceleration * glm::dvec3(right)*speedAdj;
		}
		// Strafe left
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			velocity -= acceleration * glm::dvec3(right)*speedAdj;
		}

		// up
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			velocity += acceleration * glm::dvec3(up)*speedAdj;
		}
		// down
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		{
			velocity -= acceleration * glm::dvec3(up)*speedAdj;
		}
	}
	// add friction to stop
	
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
		run = true;
	else
		run = false;

	if (run)
	{
		if (topspeed < topspeed_run) topspeed += .025f;
	}
	else
	{
		if (topspeed > topspeed_walk) topspeed -= .025f;
	}


	if (length(velocity) > topspeed) velocity = glm::normalize(velocity)*double(topspeed);

	glm::vec3 frictionDir = pow(glm::length(velocity),1.0)*glm::normalize(velocity);

	
	if (velocity.x > 0.0)
	{
		velocity.x -= friction*abs(frictionDir.x)*speedAdj;
		if (velocity.x < 0) velocity.x = 0.0;
	}
	if (velocity.y > 0.0)
	{
		velocity.y -= friction*abs(frictionDir.y)*speedAdj;
		if (velocity.y < 0) velocity.y = 0.0;
	}
	if (velocity.z > 0.0)
	{
		velocity.z -= friction*abs(frictionDir.z)*speedAdj;
		if (velocity.z < 0) velocity.z = 0.0;
	}
	if (velocity.x < 0.0)
	{
		velocity.x += friction*abs(frictionDir.x)*speedAdj;
		if (velocity.x > 0) velocity.x = 0.0;
	}
	if (velocity.y < 0.0)
	{
		velocity.y += friction*abs(frictionDir.y)*speedAdj;
		if (velocity.y > 0) velocity.y = 0.0;
	}
	if (velocity.z < 0.0)
	{
		velocity.z += friction*abs(frictionDir.z)*speedAdj;
		if (velocity.z > 0) velocity.z = 0.0;
	}
	
	
	last_position = position;
	position.x += velocity.x*speedAdj;
	position.z += velocity.z*speedAdj;
	if (cameraMode == CAM_FLY) position.y += velocity.y*speedAdj;

}

void Controls::Init(GLFWwindow *wnd)
{
	window = wnd;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Controls::updateKeystate(unsigned short key)
{
	bool state = glfwGetKey(window, key);
	
	if (state && !keystate[key].down)
	{
		keystate[key].down = true;
		keystate[key].downtimer = 0;
		return;
	}
	if (state && keystate[key].down)
	{
		keystate[key].down = true;
		keystate[key].downtimer++;
		return;
	}
	if (!state)
	{
		keystate[key].down = false;
		return;
	}
}

bool Controls::hoverRegion(glm::vec2 p0, glm::vec2 p1)
{
	if (cursorPos.x >= p0.x && cursorPos.x <= p1.x)
		if (cursorPos.y >= p0.y && cursorPos.y <= p1.y)
			return true;
	return false;
}

bool Controls::clickRegion(glm::vec2 p0, glm::vec2 p1)
{
	if (downTimer > 0)
		if (mouseDownPos.x >= p0.x && mouseDownPos.x <= p1.x)
			if (mouseDownPos.y >= p0.y && mouseDownPos.y <= p1.y)
				if (mouseUpPos.x >= p0.x && mouseUpPos.x <= p1.x)
					if (mouseUpPos.y >= p0.y && mouseUpPos.y <= p1.y)
					{
						downTimer = 0;
						mouseDownPos = glm::vec2(-2);
						mouseUpPos = glm::vec2(-2);
						return true;

					}
	return false;
}

// determine if mouse left button is down while cursor is in specified region
bool Controls::grabRegion(glm::vec2 p0, glm::vec2 p1)
{
	if (downTimer > 0 && mouse_leftDown)
		if (mouseDownPos.x >= p0.x && mouseDownPos.x <= p1.x)
			if (mouseDownPos.y >= p0.y && mouseDownPos.y <= p1.y)
			{
				return true;
			}
	return false;
}

// determine if mouse left button has pressed and released outside of specified region
bool Controls::clickOutsideRegion(glm::vec2 p0, glm::vec2 p1)
{
	if (downTimer > 0)
	{
		if (glm::length(mouseDownPos - mouseUpPos) < 10)
		{
			if (mouseDownPos.x <= p0.x || mouseDownPos.x >= p1.x) return true;
			if (mouseDownPos.y <= p0.y || mouseDownPos.y >= p1.y) return true;
			if (mouseUpPos.x <= p0.x || mouseUpPos.x >= p1.x) return true;
			if (mouseUpPos.y <= p0.y || mouseUpPos.y >= p1.y) return true;
		}
	}

	return false;
}


void Controls::typeInput()
{

	short int len = strlen(typeIn);

	//backspace key
	if (glfwGetKey(window, GLFW_KEY_BACKSPACE))
	{
		if (keystateTimer[GLFW_KEY_BACKSPACE] == 0 || keystateTimer[GLFW_KEY_BACKSPACE] >= 30)
		{
			//short int len = strlen(typeIn);
			if (len > 0)
			{
				char temp[1024];

				strncpy(temp, typeIn, len - 1);
				temp[len - 1] = '\0';
				strcpy(typeIn, temp);
			}
		}
		keystateTimer[GLFW_KEY_BACKSPACE]++;
	}
	else
	{
		keystateTimer[GLFW_KEY_BACKSPACE] = 0;
	}

	if (len < 255)
	{
		// lower case characters
		if (!glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) && !glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT))
		{

			if (glfwGetKey(window, GLFW_KEY_KP_0))
			{
				if (keystateTimer[GLFW_KEY_KP_0] == 0 || keystateTimer[GLFW_KEY_KP_0] >= 30) strcat(typeIn, "0");
				keystateTimer[GLFW_KEY_KP_0]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_KP_0] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_KP_1))
			{
				if (keystateTimer[GLFW_KEY_KP_1] == 0 || keystateTimer[GLFW_KEY_KP_1] >= 30) strcat(typeIn, "1");
				keystateTimer[GLFW_KEY_KP_1]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_KP_1] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_KP_2))
			{
				if (keystateTimer[GLFW_KEY_KP_2] == 0 || keystateTimer[GLFW_KEY_KP_2] >= 30) strcat(typeIn, "2");
				keystateTimer[GLFW_KEY_KP_2]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_KP_2] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_KP_3))
			{
				if (keystateTimer[GLFW_KEY_KP_3] == 0 || keystateTimer[GLFW_KEY_KP_3] >= 30) strcat(typeIn, "3");
				keystateTimer[GLFW_KEY_KP_3]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_KP_3] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_KP_4))
			{
				if (keystateTimer[GLFW_KEY_KP_4] == 0 || keystateTimer[GLFW_KEY_KP_4] >= 30) strcat(typeIn, "4");
				keystateTimer[GLFW_KEY_KP_4]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_KP_4] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_KP_5))
			{
				if (keystateTimer[GLFW_KEY_KP_5] == 0 || keystateTimer[GLFW_KEY_KP_5] >= 30) strcat(typeIn, "5");
				keystateTimer[GLFW_KEY_KP_5]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_KP_5] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_KP_6))
			{
				if (keystateTimer[GLFW_KEY_KP_6] == 0 || keystateTimer[GLFW_KEY_KP_6] >= 30) strcat(typeIn, "6");
				keystateTimer[GLFW_KEY_KP_6]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_KP_6] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_KP_7))
			{
				if (keystateTimer[GLFW_KEY_KP_7] == 0 || keystateTimer[GLFW_KEY_KP_7] >= 30) strcat(typeIn, "7");
				keystateTimer[GLFW_KEY_KP_7]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_KP_7] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_KP_8))
			{
				if (keystateTimer[GLFW_KEY_KP_8] == 0 || keystateTimer[GLFW_KEY_KP_8] >= 30) strcat(typeIn, "8");
				keystateTimer[GLFW_KEY_KP_8]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_KP_8] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_KP_9))
			{
				if (keystateTimer[GLFW_KEY_KP_9] == 0 || keystateTimer[GLFW_KEY_KP_9] >= 30) strcat(typeIn, "9");
				keystateTimer[GLFW_KEY_KP_9]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_KP_9] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_KP_ENTER))
			{
				if (keystateTimer[GLFW_KEY_KP_ENTER] == 0 || keystateTimer[GLFW_KEY_KP_ENTER] >= 30) strcat(typeIn, "\n");
				keystateTimer[GLFW_KEY_KP_ENTER]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_KP_ENTER] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_KP_DIVIDE))
			{
				if (keystateTimer[GLFW_KEY_KP_DIVIDE] == 0 || keystateTimer[GLFW_KEY_KP_DIVIDE] >= 30) strcat(typeIn, "/");
				keystateTimer[GLFW_KEY_KP_DIVIDE]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_KP_DIVIDE] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_KP_MULTIPLY))
			{
				if (keystateTimer[GLFW_KEY_KP_MULTIPLY] == 0 || keystateTimer[GLFW_KEY_KP_MULTIPLY] >= 30) strcat(typeIn, "*");
				keystateTimer[GLFW_KEY_KP_MULTIPLY]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_KP_MULTIPLY] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT))
			{
				if (keystateTimer[GLFW_KEY_KP_SUBTRACT] == 0 || keystateTimer[GLFW_KEY_KP_SUBTRACT] >= 30) strcat(typeIn, "-");
				keystateTimer[GLFW_KEY_KP_SUBTRACT]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_KP_SUBTRACT] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_KP_ADD))
			{
				if (keystateTimer[GLFW_KEY_KP_ADD] == 0 || keystateTimer[GLFW_KEY_KP_ADD] >= 30) strcat(typeIn, "+");
				keystateTimer[GLFW_KEY_KP_ADD]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_KP_ADD] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_KP_DECIMAL))
			{
				if (keystateTimer[GLFW_KEY_KP_DECIMAL] == 0 || keystateTimer[GLFW_KEY_KP_DECIMAL] >= 30) strcat(typeIn, ".");
				keystateTimer[GLFW_KEY_KP_DECIMAL]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_KP_DECIMAL] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_0))
			{
				if (keystateTimer[GLFW_KEY_0] == 0 || keystateTimer[GLFW_KEY_0] >= 30) strcat(typeIn, "0");
				keystateTimer[GLFW_KEY_0]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_0] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_1))
			{
				if (keystateTimer[GLFW_KEY_1] == 0 || keystateTimer[GLFW_KEY_1] >= 30) strcat(typeIn, "1");
				keystateTimer[GLFW_KEY_1]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_1] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_2))
			{
				if (keystateTimer[GLFW_KEY_2] == 0 || keystateTimer[GLFW_KEY_2] >= 30) strcat(typeIn, "2");
				keystateTimer[GLFW_KEY_2]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_2] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_3))
			{
				if (keystateTimer[GLFW_KEY_3] == 0 || keystateTimer[GLFW_KEY_3] >= 30) strcat(typeIn, "3");
				keystateTimer[GLFW_KEY_3]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_3] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_4))
			{
				if (keystateTimer[GLFW_KEY_4] == 0 || keystateTimer[GLFW_KEY_4] >= 30) strcat(typeIn, "4");
				keystateTimer[GLFW_KEY_4]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_4] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_5))
			{
				if (keystateTimer[GLFW_KEY_5] == 0 || keystateTimer[GLFW_KEY_5] >= 30) strcat(typeIn, "5");
				keystateTimer[GLFW_KEY_5]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_5] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_6))
			{
				if (keystateTimer[GLFW_KEY_6] == 0 || keystateTimer[GLFW_KEY_6] >= 30) strcat(typeIn, "6");
				keystateTimer[GLFW_KEY_6]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_6] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_7))
			{
				if (keystateTimer[GLFW_KEY_7] == 0 || keystateTimer[GLFW_KEY_7] >= 30) strcat(typeIn, "7");
				keystateTimer[GLFW_KEY_7]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_7] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_8))
			{
				if (keystateTimer[GLFW_KEY_8] == 0 || keystateTimer[GLFW_KEY_8] >= 30) strcat(typeIn, "8");
				keystateTimer[GLFW_KEY_8]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_8] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_9))
			{
				if (keystateTimer[GLFW_KEY_9] == 0 || keystateTimer[GLFW_KEY_9] >= 30) strcat(typeIn, "9");
				keystateTimer[GLFW_KEY_9]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_9] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_ENTER))
			{
				if (keystateTimer[GLFW_KEY_ENTER] == 0 || keystateTimer[GLFW_KEY_ENTER] >= 30) strcat(typeIn, "\n");
				keystateTimer[GLFW_KEY_ENTER]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_ENTER] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_COMMA))
			{
				if (keystateTimer[GLFW_KEY_COMMA] == 0 || keystateTimer[GLFW_KEY_COMMA] >= 30) strcat(typeIn, ",");
				keystateTimer[GLFW_KEY_COMMA]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_COMMA] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_PERIOD))
			{
				if (keystateTimer[GLFW_KEY_PERIOD] == 0 || keystateTimer[GLFW_KEY_PERIOD] >= 30) strcat(typeIn, ".");
				keystateTimer[GLFW_KEY_PERIOD]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_PERIOD] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_SLASH))
			{
				if (keystateTimer[GLFW_KEY_SLASH] == 0 || keystateTimer[GLFW_KEY_SLASH] >= 30) strcat(typeIn, "/");
				keystateTimer[GLFW_KEY_SLASH]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_SLASH] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_MINUS))
			{
				if (keystateTimer[GLFW_KEY_MINUS] == 0 || keystateTimer[GLFW_KEY_MINUS] >= 30) strcat(typeIn, "-");
				keystateTimer[GLFW_KEY_MINUS]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_MINUS] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT))
			{
				if (keystateTimer[GLFW_KEY_GRAVE_ACCENT] == 0 || keystateTimer[GLFW_KEY_GRAVE_ACCENT] >= 30) strcat(typeIn, "`");
				keystateTimer[GLFW_KEY_GRAVE_ACCENT]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_GRAVE_ACCENT] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_SEMICOLON))
			{
				if (keystateTimer[GLFW_KEY_SEMICOLON] == 0 || keystateTimer[GLFW_KEY_SEMICOLON] >= 30) strcat(typeIn, ";");
				keystateTimer[GLFW_KEY_SEMICOLON]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_SEMICOLON] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_APOSTROPHE))
			{
				if (keystateTimer[GLFW_KEY_APOSTROPHE] == 0 || keystateTimer[GLFW_KEY_APOSTROPHE] >= 30) strcat(typeIn, "'");
				keystateTimer[GLFW_KEY_APOSTROPHE]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_APOSTROPHE] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET))
			{
				if (keystateTimer[GLFW_KEY_LEFT_BRACKET] == 0 || keystateTimer[GLFW_KEY_LEFT_BRACKET] >= 30) strcat(typeIn, "[");
				keystateTimer[GLFW_KEY_LEFT_BRACKET]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_LEFT_BRACKET] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET))
			{
				if (keystateTimer[GLFW_KEY_RIGHT_BRACKET] == 0 || keystateTimer[GLFW_KEY_RIGHT_BRACKET] >= 30) strcat(typeIn, "]");
				keystateTimer[GLFW_KEY_RIGHT_BRACKET]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_RIGHT_BRACKET] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_BACKSLASH))
			{
				if (keystateTimer[GLFW_KEY_BACKSLASH] == 0 || keystateTimer[GLFW_KEY_BACKSLASH] >= 30) strcat(typeIn, "\\");
				keystateTimer[GLFW_KEY_BACKSLASH]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_BACKSLASH] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_A))
			{
				if (keystateTimer[GLFW_KEY_A] == 0 || keystateTimer[GLFW_KEY_A] >= 30) strcat(typeIn, "a");
				keystateTimer[GLFW_KEY_A]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_A] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_B))
			{
				if (keystateTimer[GLFW_KEY_B] == 0 || keystateTimer[GLFW_KEY_B] >= 30) strcat(typeIn, "b");
				keystateTimer[GLFW_KEY_B]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_B] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_C))
			{
				if (keystateTimer[GLFW_KEY_C] == 0 || keystateTimer[GLFW_KEY_C] >= 30) strcat(typeIn, "c");
				keystateTimer[GLFW_KEY_C]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_C] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_D))
			{
				if (keystateTimer[GLFW_KEY_D] == 0 || keystateTimer[GLFW_KEY_D] >= 30) strcat(typeIn, "d");
				keystateTimer[GLFW_KEY_D]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_D] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_E))
			{
				if (keystateTimer[GLFW_KEY_E] == 0 || keystateTimer[GLFW_KEY_E] >= 30) strcat(typeIn, "e");
				keystateTimer[GLFW_KEY_E]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_E] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_F))
			{
				if (keystateTimer[GLFW_KEY_F] == 0 || keystateTimer[GLFW_KEY_F] >= 30) strcat(typeIn, "f");
				keystateTimer[GLFW_KEY_F]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_F] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_G))
			{
				if (keystateTimer[GLFW_KEY_G] == 0 || keystateTimer[GLFW_KEY_G] >= 30) strcat(typeIn, "g");
				keystateTimer[GLFW_KEY_G]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_G] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_H))
			{
				if (keystateTimer[GLFW_KEY_H] == 0 || keystateTimer[GLFW_KEY_H] >= 30) strcat(typeIn, "h");
				keystateTimer[GLFW_KEY_H]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_H] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_I))
			{
				if (keystateTimer[GLFW_KEY_I] == 0 || keystateTimer[GLFW_KEY_I] >= 30) strcat(typeIn, "i");
				keystateTimer[GLFW_KEY_I]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_I] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_J))
			{
				if (keystateTimer[GLFW_KEY_J] == 0 || keystateTimer[GLFW_KEY_J] >= 30) strcat(typeIn, "j");
				keystateTimer[GLFW_KEY_J]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_J] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_K))
			{
				if (keystateTimer[GLFW_KEY_K] == 0 || keystateTimer[GLFW_KEY_K] >= 30) strcat(typeIn, "k");
				keystateTimer[GLFW_KEY_K]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_K] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_L))
			{
				if (keystateTimer[GLFW_KEY_L] == 0 || keystateTimer[GLFW_KEY_L] >= 30) strcat(typeIn, "l");
				keystateTimer[GLFW_KEY_L]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_L] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_M))
			{
				if (keystateTimer[GLFW_KEY_M] == 0 || keystateTimer[GLFW_KEY_M] >= 30) strcat(typeIn, "m");
				keystateTimer[GLFW_KEY_M]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_M] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_N))
			{
				if (keystateTimer[GLFW_KEY_N] == 0 || keystateTimer[GLFW_KEY_N] >= 30) strcat(typeIn, "n");
				keystateTimer[GLFW_KEY_N]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_N] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_O))
			{
				if (keystateTimer[GLFW_KEY_O] == 0 || keystateTimer[GLFW_KEY_O] >= 30) strcat(typeIn, "o");
				keystateTimer[GLFW_KEY_O]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_O] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_P))
			{
				if (keystateTimer[GLFW_KEY_P] == 0 || keystateTimer[GLFW_KEY_P] >= 30) strcat(typeIn, "p");
				keystateTimer[GLFW_KEY_P]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_P] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_Q))
			{
				if (keystateTimer[GLFW_KEY_Q] == 0 || keystateTimer[GLFW_KEY_Q] >= 30) strcat(typeIn, "q");
				keystateTimer[GLFW_KEY_Q]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_Q] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_R))
			{
				if (keystateTimer[GLFW_KEY_R] == 0 || keystateTimer[GLFW_KEY_R] >= 30) strcat(typeIn, "r");
				keystateTimer[GLFW_KEY_R]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_R] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_S))
			{
				if (keystateTimer[GLFW_KEY_S] == 0 || keystateTimer[GLFW_KEY_S] >= 30) strcat(typeIn, "s");
				keystateTimer[GLFW_KEY_S]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_S] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_T))
			{
				if (keystateTimer[GLFW_KEY_T] == 0 || keystateTimer[GLFW_KEY_T] >= 30) strcat(typeIn, "t");
				keystateTimer[GLFW_KEY_T]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_T] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_U))
			{
				if (keystateTimer[GLFW_KEY_U] == 0 || keystateTimer[GLFW_KEY_U] >= 30) strcat(typeIn, "u");
				keystateTimer[GLFW_KEY_U]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_U] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_V))
			{
				if (keystateTimer[GLFW_KEY_V] == 0 || keystateTimer[GLFW_KEY_V] >= 30) strcat(typeIn, "v");
				keystateTimer[GLFW_KEY_V]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_V] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_W))
			{
				if (keystateTimer[GLFW_KEY_W] == 0 || keystateTimer[GLFW_KEY_W] >= 30) strcat(typeIn, "w");
				keystateTimer[GLFW_KEY_W]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_W] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_X))
			{
				if (keystateTimer[GLFW_KEY_X] == 0 || keystateTimer[GLFW_KEY_X] >= 30) strcat(typeIn, "x");
				keystateTimer[GLFW_KEY_X]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_X] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_Y))
			{
				if (keystateTimer[GLFW_KEY_Y] == 0 || keystateTimer[GLFW_KEY_Y] >= 30) strcat(typeIn, "y");
				keystateTimer[GLFW_KEY_Y]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_Y] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_Z))
			{
				if (keystateTimer[GLFW_KEY_Z] == 0 || keystateTimer[GLFW_KEY_Z] >= 30) strcat(typeIn, "z");
				keystateTimer[GLFW_KEY_Z]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_Z] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_SPACE))
			{
				if (keystateTimer[GLFW_KEY_SPACE] == 0 || keystateTimer[GLFW_KEY_SPACE] >= 30) strcat(typeIn, " ");
				keystateTimer[GLFW_KEY_SPACE]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_SPACE] = 0;
			}

		}

		// upper case characters
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT))
		{

			if (glfwGetKey(window, GLFW_KEY_0))
			{
				if (keystateTimer[GLFW_KEY_0] == 0 || keystateTimer[GLFW_KEY_0] >= 30) strcat(typeIn, ")");
				keystateTimer[GLFW_KEY_0]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_0] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_1))
			{
				if (keystateTimer[GLFW_KEY_1] == 0 || keystateTimer[GLFW_KEY_1] >= 30) strcat(typeIn, "!");
				keystateTimer[GLFW_KEY_1]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_1] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_2))
			{
				if (keystateTimer[GLFW_KEY_2] == 0 || keystateTimer[GLFW_KEY_2] >= 30) strcat(typeIn, "@");
				keystateTimer[GLFW_KEY_2]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_2] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_3))
			{
				if (keystateTimer[GLFW_KEY_3] == 0 || keystateTimer[GLFW_KEY_3] >= 30) strcat(typeIn, "#");
				keystateTimer[GLFW_KEY_3]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_3] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_4))
			{
				if (keystateTimer[GLFW_KEY_4] == 0 || keystateTimer[GLFW_KEY_4] >= 30) strcat(typeIn, "$");
				keystateTimer[GLFW_KEY_4]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_4] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_5))
			{
				if (keystateTimer[GLFW_KEY_5] == 0 || keystateTimer[GLFW_KEY_5] >= 30) strcat(typeIn, "%");
				keystateTimer[GLFW_KEY_5]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_5] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_6))
			{
				if (keystateTimer[GLFW_KEY_6] == 0 || keystateTimer[GLFW_KEY_6] >= 30) strcat(typeIn, "^");
				keystateTimer[GLFW_KEY_6]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_6] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_7))
			{
				if (keystateTimer[GLFW_KEY_7] == 0 || keystateTimer[GLFW_KEY_7] >= 30) strcat(typeIn, "&");
				keystateTimer[GLFW_KEY_7]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_7] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_8))
			{
				if (keystateTimer[GLFW_KEY_8] == 0 || keystateTimer[GLFW_KEY_8] >= 30) strcat(typeIn, "*");
				keystateTimer[GLFW_KEY_8]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_8] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_9))
			{
				if (keystateTimer[GLFW_KEY_9] == 0 || keystateTimer[GLFW_KEY_9] >= 30) strcat(typeIn, "(");
				keystateTimer[GLFW_KEY_9]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_9] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_COMMA))
			{
				if (keystateTimer[GLFW_KEY_COMMA] == 0 || keystateTimer[GLFW_KEY_COMMA] >= 30) strcat(typeIn, "<");
				keystateTimer[GLFW_KEY_COMMA]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_COMMA] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_PERIOD))
			{
				if (keystateTimer[GLFW_KEY_PERIOD] == 0 || keystateTimer[GLFW_KEY_PERIOD] >= 30) strcat(typeIn, ">");
				keystateTimer[GLFW_KEY_PERIOD]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_PERIOD] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_SLASH))
			{
				if (keystateTimer[GLFW_KEY_SLASH] == 0 || keystateTimer[GLFW_KEY_SLASH] >= 30) strcat(typeIn, "?");
				keystateTimer[GLFW_KEY_SLASH]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_SLASH] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_MINUS))
			{
				if (keystateTimer[GLFW_KEY_MINUS] == 0 || keystateTimer[GLFW_KEY_MINUS] >= 30) strcat(typeIn, "_");
				keystateTimer[GLFW_KEY_MINUS]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_MINUS] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT))
			{
				if (keystateTimer[GLFW_KEY_GRAVE_ACCENT] == 0 || keystateTimer[GLFW_KEY_GRAVE_ACCENT] >= 30) strcat(typeIn, "~");
				keystateTimer[GLFW_KEY_GRAVE_ACCENT]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_GRAVE_ACCENT] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_SEMICOLON))
			{
				if (keystateTimer[GLFW_KEY_SEMICOLON] == 0 || keystateTimer[GLFW_KEY_SEMICOLON] >= 30) strcat(typeIn, ":");
				keystateTimer[GLFW_KEY_SEMICOLON]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_SEMICOLON] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_APOSTROPHE))
			{
				if (keystateTimer[GLFW_KEY_APOSTROPHE] == 0 || keystateTimer[GLFW_KEY_APOSTROPHE] >= 30) strcat(typeIn, "\"");
				keystateTimer[GLFW_KEY_APOSTROPHE]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_APOSTROPHE] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET))
			{
				if (keystateTimer[GLFW_KEY_LEFT_BRACKET] == 0 || keystateTimer[GLFW_KEY_LEFT_BRACKET] >= 30) strcat(typeIn, "{");
				keystateTimer[GLFW_KEY_LEFT_BRACKET]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_LEFT_BRACKET] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET))
			{
				if (keystateTimer[GLFW_KEY_RIGHT_BRACKET] == 0 || keystateTimer[GLFW_KEY_RIGHT_BRACKET] >= 30) strcat(typeIn, "}");
				keystateTimer[GLFW_KEY_RIGHT_BRACKET]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_RIGHT_BRACKET] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_BACKSLASH))
			{
				if (keystateTimer[GLFW_KEY_BACKSLASH] == 0 || keystateTimer[GLFW_KEY_BACKSLASH] >= 30) strcat(typeIn, "|");
				keystateTimer[GLFW_KEY_BACKSLASH]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_BACKSLASH] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_A))
			{
				if (keystateTimer[GLFW_KEY_A] == 0 || keystateTimer[GLFW_KEY_A] >= 30) strcat(typeIn, "A");
				keystateTimer[GLFW_KEY_A]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_A] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_B))
			{
				if (keystateTimer[GLFW_KEY_B] == 0 || keystateTimer[GLFW_KEY_B] >= 30) strcat(typeIn, "B");
				keystateTimer[GLFW_KEY_B]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_B] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_C))
			{
				if (keystateTimer[GLFW_KEY_C] == 0 || keystateTimer[GLFW_KEY_C] >= 30) strcat(typeIn, "C");
				keystateTimer[GLFW_KEY_C]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_C] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_D))
			{
				if (keystateTimer[GLFW_KEY_D] == 0 || keystateTimer[GLFW_KEY_D] >= 30) strcat(typeIn, "D");
				keystateTimer[GLFW_KEY_D]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_D] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_E))
			{
				if (keystateTimer[GLFW_KEY_E] == 0 || keystateTimer[GLFW_KEY_E] >= 30) strcat(typeIn, "E");
				keystateTimer[GLFW_KEY_E]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_E] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_F))
			{
				if (keystateTimer[GLFW_KEY_F] == 0 || keystateTimer[GLFW_KEY_F] >= 30) strcat(typeIn, "F");
				keystateTimer[GLFW_KEY_F]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_F] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_G))
			{
				if (keystateTimer[GLFW_KEY_G] == 0 || keystateTimer[GLFW_KEY_G] >= 30) strcat(typeIn, "G");
				keystateTimer[GLFW_KEY_G]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_G] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_H))
			{
				if (keystateTimer[GLFW_KEY_H] == 0 || keystateTimer[GLFW_KEY_H] >= 30) strcat(typeIn, "H");
				keystateTimer[GLFW_KEY_H]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_H] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_I))
			{
				if (keystateTimer[GLFW_KEY_I] == 0 || keystateTimer[GLFW_KEY_I] >= 30) strcat(typeIn, "I");
				keystateTimer[GLFW_KEY_I]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_I] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_J))
			{
				if (keystateTimer[GLFW_KEY_J] == 0 || keystateTimer[GLFW_KEY_J] >= 30) strcat(typeIn, "J");
				keystateTimer[GLFW_KEY_J]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_J] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_K))
			{
				if (keystateTimer[GLFW_KEY_K] == 0 || keystateTimer[GLFW_KEY_K] >= 30) strcat(typeIn, "K");
				keystateTimer[GLFW_KEY_K]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_K] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_L))
			{
				if (keystateTimer[GLFW_KEY_L] == 0 || keystateTimer[GLFW_KEY_L] >= 30) strcat(typeIn, "L");
				keystateTimer[GLFW_KEY_L]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_L] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_M))
			{
				if (keystateTimer[GLFW_KEY_M] == 0 || keystateTimer[GLFW_KEY_M] >= 30) strcat(typeIn, "M");
				keystateTimer[GLFW_KEY_M]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_M] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_N))
			{
				if (keystateTimer[GLFW_KEY_N] == 0 || keystateTimer[GLFW_KEY_N] >= 30) strcat(typeIn, "N");
				keystateTimer[GLFW_KEY_N]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_N] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_O))
			{
				if (keystateTimer[GLFW_KEY_O] == 0 || keystateTimer[GLFW_KEY_O] >= 30) strcat(typeIn, "O");
				keystateTimer[GLFW_KEY_O]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_O] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_P))
			{
				if (keystateTimer[GLFW_KEY_P] == 0 || keystateTimer[GLFW_KEY_P] >= 30) strcat(typeIn, "P");
				keystateTimer[GLFW_KEY_P]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_P] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_Q))
			{
				if (keystateTimer[GLFW_KEY_Q] == 0 || keystateTimer[GLFW_KEY_Q] >= 30) strcat(typeIn, "Q");
				keystateTimer[GLFW_KEY_Q]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_Q] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_R))
			{
				if (keystateTimer[GLFW_KEY_R] == 0 || keystateTimer[GLFW_KEY_R] >= 30) strcat(typeIn, "R");
				keystateTimer[GLFW_KEY_R]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_R] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_S))
			{
				if (keystateTimer[GLFW_KEY_S] == 0 || keystateTimer[GLFW_KEY_S] >= 30) strcat(typeIn, "S");
				keystateTimer[GLFW_KEY_S]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_S] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_T))
			{
				if (keystateTimer[GLFW_KEY_T] == 0 || keystateTimer[GLFW_KEY_T] >= 30) strcat(typeIn, "T");
				keystateTimer[GLFW_KEY_T]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_T] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_U))
			{
				if (keystateTimer[GLFW_KEY_U] == 0 || keystateTimer[GLFW_KEY_U] >= 30) strcat(typeIn, "U");
				keystateTimer[GLFW_KEY_U]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_U] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_V))
			{
				if (keystateTimer[GLFW_KEY_V] == 0 || keystateTimer[GLFW_KEY_V] >= 30) strcat(typeIn, "V");
				keystateTimer[GLFW_KEY_V]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_V] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_W))
			{
				if (keystateTimer[GLFW_KEY_W] == 0 || keystateTimer[GLFW_KEY_W] >= 30) strcat(typeIn, "W");
				keystateTimer[GLFW_KEY_W]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_W] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_X))
			{
				if (keystateTimer[GLFW_KEY_X] == 0 || keystateTimer[GLFW_KEY_X] >= 30) strcat(typeIn, "X");
				keystateTimer[GLFW_KEY_X]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_X] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_Y))
			{
				if (keystateTimer[GLFW_KEY_Y] == 0 || keystateTimer[GLFW_KEY_Y] >= 30) strcat(typeIn, "Y");
				keystateTimer[GLFW_KEY_Y]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_Y] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_Z))
			{
				if (keystateTimer[GLFW_KEY_Z] == 0 || keystateTimer[GLFW_KEY_Z] >= 30) strcat(typeIn, "Z");
				keystateTimer[GLFW_KEY_Z]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_Z] = 0;
			}

			if (glfwGetKey(window, GLFW_KEY_SPACE))
			{
				if (keystateTimer[GLFW_KEY_SPACE] == 0 || keystateTimer[GLFW_KEY_SPACE] >= 30) strcat(typeIn, " ");
				keystateTimer[GLFW_KEY_SPACE]++;
			}
			else
			{
				keystateTimer[GLFW_KEY_SPACE] = 0;
			}
		}
	}
}
