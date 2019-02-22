#ifndef CAMERA_H_
#define CAMERA_H_

#include "glm.hpp"


struct Camera
{
	Camera()
	{
		pos = glm::vec3(0);
		rot = glm::vec3(0);
		FOV = 90.0f;
		aspect = 1.0f;
		nearPlane = .1f;
		farPlane = 2000.0f;
		orthoWidth = 10.0f;
		orthoHeight = 10.0f;

	}
	Camera(const Camera &other)
	{
		pos = other.pos;
		rot = other.rot;
		FOV = other.FOV;
		aspect = other.aspect;
		nearPlane = other.nearPlane;
		farPlane = other.farPlane;
		orthoWidth = other.orthoWidth;
		orthoHeight = other.orthoHeight;
		transformMatrix = other.transformMatrix;
		up = other.up;
		forward = other.forward;
		right = other.right;

		projMatrix = other.projMatrix;
		viewMatrix = other.viewMatrix;
		view_projMatrix = other.view_projMatrix;

	}

	glm::vec3 pos;
	glm::vec3 rot;
	float FOV;
	float aspect;
	float nearPlane;
	float farPlane;
	float orthoWidth;
	float orthoHeight;

	glm::vec3 forward;
	glm::vec3 up;
	glm::vec3 right;

	void setFOV(float fov);
	float getFOV();

	void setPosition(glm::vec3 p);
	void setPosition(float x, float y, float z);
	glm::vec3 getPosition();

	void setRotation(glm::vec3 p);
	void setRotation(float x, float y, float z);
	glm::vec3 getRotation();

	void setCameraAspect(float ratio);
	float getCameraAspect();

	void setCameraRange(float nearplane, float farplane);

	void setOrthoSize(float width, float height);
	float getOrthoWidth();
	float getOrthoHeight();

	glm::vec3 getForwardVector();
	glm::vec3 getRightVector();
	glm::vec3 getUpVector();

	glm::mat4 updateTransformMatrix();
	void updateLocalVectors();

	glm::mat4 transformMatrix;

	glm::mat4 projMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 view_projMatrix;
};

struct CameraManager
{
	CameraManager();

	Camera *camArray;
	unsigned short arraySize;
	unsigned short camCount;

	int add(Camera cam);
	bool remove(int index);
	static glm::vec3 lookatAngle(Camera cam, glm::vec3 lookatpos);
	static glm::mat4 lookatMatrix(Camera cam, glm::vec3 lookatpos, float z_rotation);	
	static glm::mat4 lookatMatrix(glm::vec3 angle);
	static glm::vec3 getForwardVector(Camera cam);
	static glm::vec3 getRightVector(Camera cam);
	static glm::vec3 getUpVector(Camera cam);

	

	// define the plane to mirror the camera about. the offset would be the d component of the plane
	static Camera reflectCam(Camera cam, glm::vec3 surfaceNorm, float surfaceOffset);
};


#endif
