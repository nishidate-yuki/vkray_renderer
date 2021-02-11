#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

class Application;

enum class PressState
{
    NONE,
    PRESSED,
    HELD,
    RELEASED
};

class InputSystem
{
public:

    InputSystem() = delete;
    ~InputSystem() = delete;

    static void initialize(Application* app, GLFWwindow* window);

    static glm::vec2 getCursorPos();
    static glm::vec2 getLastCursorPos();
    static glm::vec2 getCusorMotion();
    static PressState getButtonState(int button);
    static int getKey(int key);

    static void setLastCursorPos(glm::vec2 pos);
    static void setCusorMotion(glm::vec2 motion);
    static void setButtons(int button, PressState state);

private:

    static Application* app;
    static GLFWwindow* window;

    static glm::vec2 lastCusorPos;
    static glm::vec2 cusorMotion;

    static PressState buttons[10];
    static PressState keys[1024];
};

