#pragma once

#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

//enum class CameraType
//{
//    ORBITAL
//};

struct Camera
{
    glm::vec4 position;
    glm::vec3 target;

    glm::mat4 view;
    glm::mat4 proj;

    float fov = 45;
    float aspect = 1;

    float phi = 0;
    float theta = 0;

    // for fps
    glm::vec3 front;
    glm::vec3 up;
    float rotSpeed = 0.1;
    float moveSpeed = 0.1;

    float pitch = 0;
    float yaw = 0;


    Camera();

    Camera(int width, int height);

    void update();

    void processCursorMotion(glm::vec2 cursorMotion);

    void processMouseWheel(float value);

    void processKeyState();
};
