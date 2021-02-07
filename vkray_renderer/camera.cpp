#include "camera.h"

#include <algorithm>

#include <glm/gtc/constants.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

Camera::Camera(float width, float height)
    : position{ 0, 0, 10, 1 }
    , aspect{ width / height }
    , fov{ 45 }
    , znear{ 0.001f }
    , zfar{ 100000.0f }
    , rotation{ 0.0f }
{
    update();
}

glm::mat4 Camera::getInvView()
{
    return glm::inverse(viewMatrix);
}

glm::mat4 Camera::getInvProj()
{
    return glm::inverse(projMatrix);
}

void Camera::update()
{
    auto rotX = glm::rotate(glm::radians(rotation.x), glm::vec3{ 1, 0, 0 });
    auto rotY = glm::rotate(glm::radians(rotation.y), glm::vec3{ 0, 1, 0 });
    auto front = glm::vec3(rotY * rotX * glm::vec4{ 0 });

    viewMatrix = glm::lookAt(glm::vec3(rotY * rotX * position), front, glm::vec3{ 0, 1, 0 });
    projMatrix = glm::perspective(glm::radians(fov), aspect, znear, zfar);
}

void Camera::processMouseMotion(float dx, float dy)
{
    rotation.x = std::clamp(rotation.x + dy, -89.9f, 89.9f);
    rotation.y = glm::mod(rotation.y - dx, 360.0f);
    update();
}

void Camera::processMouseWheel(float value)
{
    position.z -= value;
    update();
}
