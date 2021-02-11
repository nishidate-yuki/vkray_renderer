#include "camera.h"

#include <iostream>

#include "input_system.h"

Camera::Camera()
{
}

Camera::Camera(int width, int height)
{
    position = glm::vec4(0, -3, 20, 1);
    //target = glm::vec3(0, -3, 0);
    front = glm::vec3(0, -3, 0) - glm::vec3(position);
    up = glm::vec3(0, 1, 0);
    aspect = float(width) / height;
    proj = glm::perspective(glm::radians(fov), aspect, 0.01f, 10000.0f);

    update();
}

void Camera::update()
{
    glm::mat4 rotationMatrix(1.0);
    rotationMatrix *= glm::rotate(glm::radians(yaw), glm::vec3(0, 1, 0));
    rotationMatrix *= glm::rotate(glm::radians(pitch), glm::vec3(1, 0, 0));
    front = glm::vec3(rotationMatrix * glm::vec4(0, 0, -1, 1));

    view = glm::lookAt(glm::vec3(position), glm::vec3(position) + front, up);
    //proj = glm::perspective(glm::radians(fov), aspect, 0.01f, 10000.0f);

    //glm::mat4 rotX = glm::rotate(glm::radians(theta), glm::vec3(1, 0, 0));
    //glm::mat4 rotY = glm::rotate(glm::radians(phi), glm::vec3(0, 1, 0));

    //view = glm::lookAt(glm::vec3(rotY * rotX * position), target, glm::vec3(0, 1, 0));
    //proj = glm::perspective(glm::radians(fov), aspect, 0.01f, 10000.0f);
}

void Camera::processCursorMotion(glm::vec2 cursorMotion)
{
    //phi = glm::mod(phi - dx, 360.0f);
    //theta = std::min(std::max(theta + dy, -89.9f), 89.9f);

    yaw = glm::mod(yaw - cursorMotion.x * rotSpeed, 360.0f);
    pitch = std::min(std::max(pitch + cursorMotion.y * rotSpeed, -89.9f), 89.9f);
    update();
}

void Camera::processMouseWheel(float value)
{
    //position.z = std::max(position.z - value, 0.001f);
    //update();
}

void Camera::processKeyState()
{
    glm::vec3 forward = glm::normalize(glm::vec3(front.x, 0, front.z));
    glm::vec3 right = glm::normalize(glm::cross(-up, forward));

    if (InputSystem::getKey(GLFW_KEY_W) == GLFW_PRESS) {
        position += glm::vec4(forward, 0.0) * moveSpeed;
    }
    if (InputSystem::getKey(GLFW_KEY_S) == GLFW_PRESS) {
        position -= glm::vec4(forward, 0.0) * moveSpeed;
    }
    if (InputSystem::getKey(GLFW_KEY_D) == GLFW_PRESS) {
        position += glm::vec4(right, 0.0) * moveSpeed * 0.5f;
    }
    if (InputSystem::getKey(GLFW_KEY_A) == GLFW_PRESS) {
        position -= glm::vec4(right, 0.0) * moveSpeed * 0.5f;
    }
    update();
}
