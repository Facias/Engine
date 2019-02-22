#include "Camera.h"
#include <iostream>

void Camera::setFOV(float fov)
{
	FOV = fov;
}

float Camera::getFOV()
{
	return FOV;
}

void Camera::setPosition(glm::vec3 p)
{
	pos = p;
}

void Camera::setPosition(float x, float y, float z)
{
	pos = glm::vec3(x, y, z);
}

glm::vec3 Camera::getPosition()
{
	return pos;
}

void Camera::setRotation(glm::vec3 p)
{
	rot = p;
}

void Camera::setRotation(float x, float y, float z)
{
	rot = glm::vec3(x, y, z);
}

glm::vec3 Camera::getRotation()
{
	return rot;
}

void Camera::setCameraAspect(float ratio)
{
	aspect = ratio;
}

float Camera::getCameraAspect()
{
	return aspect;
}

void Camera::setCameraRange(float nearplane, float farplane)
{
	nearPlane = nearplane;
	farPlane = farplane;
}

void Camera::setOrthoSize(float width, float height)
{
	orthoWidth = width;
	orthoHeight = height;
}

float Camera::getOrthoWidth()
{
	return orthoWidth;
}

float Camera::getOrthoHeight()
{
	return orthoHeight;
}

glm::vec3 Camera::getForwardVector()
{
	glm::vec4 p = transformMatrix * glm::vec4(0, 0, 1, 1);
	return glm::vec3(p.x, p.y, p.z);
}

glm::vec3 Camera::getRightVector()
{
	glm::vec4 p = transformMatrix * glm::vec4(1, 0, 0, 1);
	return glm::vec3(p.x, p.y, p.z);
}

glm::vec3 Camera::getUpVector()
{
	glm::vec4 p = transformMatrix * glm::vec4(0, 1, 0, 1);
	return glm::vec3(p.x, p.y, p.z);
}

void Camera::updateLocalVectors()
{
	forward = getForwardVector();
	right = getRightVector();
	up = getUpVector();
}




// camera manager methods

CameraManager::CameraManager()
{
	camArray = new Camera[4];
	arraySize = 4;
	camCount = 0;
}

int CameraManager::add(Camera cam)
{
	camArray[camCount++] = cam;

	if (camCount + 2 > arraySize)
	{
		Camera *temp;
		temp = new Camera[arraySize * 2];

		for (int n = 0; n < camCount; n++)
			temp[n] = camArray[n];

		delete[] camArray;
		camArray = temp;

		return camCount - 1;
	}
}

Camera CameraManager::reflectCam(Camera cam, glm::vec3 surfaceNorm, float surfaceOffset)
{
	Camera temp;

	//find reflected camera position
	glm::vec3 raydir = -surfaceNorm;
	float t = (glm::dot(-cam.pos, surfaceNorm) + surfaceOffset);
	temp.pos = cam.pos + t*2.0f*surfaceNorm;
	temp.forward = cam.forward;
	temp.forward.y *= -1;
	temp.forward.x *= 1;

	//glm::vec3 angle = glm::vec3(-cam.rot.x, cam.rot.y, 0);//lookatAngle(temp, lookatpos);

	
	return temp;
}

glm::vec3 CameraManager::lookatAngle(Camera cam, glm::vec3 lookatpos)
{
	glm::vec3 lookvec = lookatpos - cam.pos;
	float x_rotation = atan2(lookvec.y, glm::length(glm::vec2(lookvec.x, lookvec.z)));
	float y_rotation = atan2(lookvec.x, lookvec.z);

	glm::vec3 rot = glm::vec3(x_rotation, y_rotation, 0);

	return rot;
}

glm::mat4 CameraManager::lookatMatrix(Camera cam, glm::vec3 lookatpos, float z_rotation)
{
	glm::vec3 lookvec =  lookatpos - cam.pos;
	float x_rotation = atan2(-lookvec.y, glm::length(glm::vec2(lookvec.x, lookvec.z))) ;
	float y_rotation = atan2(lookvec.x, -lookvec.z);

	glm::mat4 xrotation, yrotation, zrotation , position;
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
	
	position = glm::mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		-cam.pos.x, -cam.pos.y, -cam.pos.z, 1.0);


	return zrotation *xrotation *yrotation* position;
}

glm::mat4 CameraManager::lookatMatrix( glm::vec3 angle)
{
	glm::mat4 xrotation, yrotation, zrotation;
	xrotation = glm::mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, cos(angle.x), sin(angle.x), 0.0,
		0.0, -sin(angle.x), cos(angle.x), 0.0,
		0.0, 0.0, 0.0, 1.0);

	yrotation = glm::mat4(
		cos(angle.y), 0.0, -sin(angle.y), 0.0,
		0.0, 1.0, 0.0, 0.0,
		sin(angle.y), 0.0, cos(angle.y), 0.0,
		0.0, 0.0, 0.0, 1.0);

	zrotation = glm::mat4(
		cos(angle.z), sin(angle.z), 0.0, 0.0,
		-sin(angle.z), cos(angle.z), 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0);


	return xrotation *yrotation*zrotation;
}

glm::vec3 CameraManager::getForwardVector(Camera cam)
{
	glm::vec4 p = cam.transformMatrix * glm::vec4(0,0,1,1);
	return glm::vec3(p.x, p.y, p.z);
}

glm::vec3 CameraManager::getRightVector(Camera cam)
{
	glm::vec4 p = cam.transformMatrix * glm::vec4(1, 0, 0, 1);
	return glm::vec3(p.x, p.y, p.z);
}

glm::vec3 CameraManager::getUpVector(Camera cam)
{
	glm::vec4 p = cam.transformMatrix * glm::vec4(0, 1, 0, 1);
	return glm::vec3(p.x, p.y, p.z);
}

