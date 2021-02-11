#pragma once

#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


class Camera
{
public:

    virtual ~Camera() = default;

    glm::vec4 position;

    glm::mat4 view;
    glm::mat4 proj;

    glm::vec3 up;

    float fov = 45;
    float aspect = 1;

    virtual void update() = 0;

    virtual void processCursorMotion(glm::vec2 cursorMotion) {}
    virtual void processMouseWheel(float value) {}
    virtual void processKeyState() {}
};


class OrbitalCamera : public Camera
{
public:

    glm::vec3 target;

    float phi = 0;
    float theta = 0;

    OrbitalCamera(int width, int height);

    void update() override;

    void processCursorMotion(glm::vec2 cursorMotion) override;
    void processMouseWheel(float value) override;
};

class FPSCamera : public Camera
{
public:

    glm::vec3 front;

    float rotSpeed = 0.1;
    float moveSpeed = 0.1;

    float pitch = 0;
    float yaw = 0;

    FPSCamera(int width, int height);

    void update() override;

    void processCursorMotion(glm::vec2 cursorMotion) override;
    void processMouseWheel(float value) override;
    void processKeyState() override;
};
