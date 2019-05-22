#include "Camera.h"

extern GLfloat auxk = 0.0f;

Camera::Camera() {}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	moveSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;

	update();
}

void Camera::keyControl(bool* keys, GLfloat deltaTime)
{
	GLfloat velocity = moveSpeed * deltaTime;

	if (keys[GLFW_KEY_W])
	{
		position += front * velocity;
	}

	if (keys[GLFW_KEY_S])
	{
		position -= front * velocity;
	}

	if (keys[GLFW_KEY_A])
	{
		position -= right * velocity;
	}

	if (keys[GLFW_KEY_D])
	{
		position += right * velocity;
	}
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}

	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	update();
}

glm::mat4 Camera::calculateViewMatrix()
{
	if (CAM_ACTIVATE == 1.0f) {
		return glm::lookAt(glm::vec3(0.0f, 40.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else if (CAM_ACTIVATE == 2.0f)
	{
		return glm::lookAt(glm::vec3(-12.0f, 0.0f, 5.0f), glm::vec3(-6.5f, 2.55f, -4.8f) , glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (CAM_ACTIVATE == 3.0f)
	{
		return glm::lookAt(glm::vec3(13.0f, 0.0f, -5.0f), glm::vec3(4.5f, 2.55f, -11.8f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (CAM_ACTIVATE == 4.0f)
	{
		return glm::lookAt(glm::vec3(4.5f, auxk, -10.0f), glm::vec3(4.5f, auxk, 3.27f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else
	{
		return glm::lookAt(position, position + front, up);
	}
}

glm::vec3 Camera::getCameraPosition()
{
	return position;
}

glm::vec3 Camera::getCameraDirection()
{
	return glm::normalize(front);
}

void Camera::update()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}


Camera::~Camera()
{
}
