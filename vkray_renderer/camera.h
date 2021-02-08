#pragma once

#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

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

    Camera();

    Camera(int width, int height);

    void update();

    void processMouseMotion(float dx, float dy);

    void processMouseWheel(float value);
};
