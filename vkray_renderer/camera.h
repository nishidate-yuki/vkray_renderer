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

    Camera() = default;

    Camera(float width, float height);

    glm::mat4 getInvView();

    glm::mat4 getInvProj();

    void update();

    void processMouseMotion(float dx, float dy);

    void processMouseWheel(float value);

protected:

    glm::vec4 position;
    glm::vec3 rotation;

    float aspect;
    float fov;
    float znear;
    float zfar;

    glm::mat4 viewMatrix;
    glm::mat4 projMatrix;
};
