#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

class Application;

class InputSystem
{
public:

    InputSystem() = delete;
    ~InputSystem() = delete;

    static void initialize(Application* app, GLFWwindow* window);

    static glm::vec2 getCursorPos();
    static int getKey(int key);

private:

    static Application* app;
    static GLFWwindow* window;

};

