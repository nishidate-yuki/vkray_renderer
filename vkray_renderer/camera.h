#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class Camera
{
public:

	Camera(float width, float height)
		: aspect(width / height)
		, pos{ 0, 0, 10, 1 }
	{
		invView = glm::inverse(glm::lookAt(glm::vec3(pos), target, up));
		invProj = glm::inverse(glm::perspective(glm::radians(fov), aspect, znear, zfar));
	}

private:

	float fov{ 45 };
	float aspect;
	float znear{ 0.001f };
	float zfar{ 100000.0f };

	glm::vec4 pos;
	glm::vec3 target;
	glm::vec3 up{ 0, 1, 0 };

	glm::mat4 invView{ 1 };
	glm::mat4 invProj{ 1 };


	void update(float dx, float dy)
	{
		phi -= dx;
		theta = std::min(std::max(theta + dy, -89.0f), 89.0f);

		rotX = glm::rotate(glm::radians(theta), glm::vec3(1, 0, 0));
		rotY = glm::rotate(glm::radians(phi), glm::vec3(0, 1, 0));

		invView = glm::inverse(glm::lookAt(glm::vec3(rotY * rotX * pos), target, up));
		invProj = glm::inverse(glm::perspective(glm::radians(fov), aspect, znear, zfar));
	}

	void update(float yoffset)
	{
		pos.z -= yoffset / 4.0 * scale;
		invView = glm::inverse(glm::lookAt(glm::vec3(rotY * rotX * pos), target, up));
		invProj = glm::inverse(glm::perspective(glm::radians(fov), aspect, znear, zfar));
	}
};