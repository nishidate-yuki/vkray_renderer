#include "camera.h"

Camera::Camera()
{
    position = glm::vec4(0, 0, 20, 1);
    target = glm::vec3(0);
    update();
}

Camera::Camera(int width, int height)
{
    position = glm::vec4(0, 0, 20, 1);
    target = glm::vec3(0);
    aspect = float(width) / height;
    update();
}

void Camera::update()
{
    glm::mat4 rotX = glm::rotate(glm::radians(theta), glm::vec3(1, 0, 0));
    glm::mat4 rotY = glm::rotate(glm::radians(phi), glm::vec3(0, 1, 0));

    view = glm::lookAt(glm::vec3(rotY * rotX * position), target, glm::vec3(0, 1, 0));
    proj = glm::perspective(glm::radians(fov), aspect, 0.01f, 10000.0f);
}

void Camera::processMouseMotion(float dx, float dy)
{
    phi = glm::mod(phi - dx, 360.0f);
    theta = std::min(std::max(theta + dy, -89.9f), 89.9f);
    update();
}

void Camera::processMouseWheel(float value)
{
    position.z = std::max(position.z - value, 0.001f);
    update();
}
