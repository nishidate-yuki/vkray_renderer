#include "input_system.h"

#include "application.h"

namespace
{
    void keyCallback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods)
    {
        auto* const this_ = static_cast<Application*>(glfwGetWindowUserPointer(window));
    }

    void cursorPositionCallback(GLFWwindow* window, const double xpos, const double ypos)
    {
        auto* const this_ = static_cast<Application*>(glfwGetWindowUserPointer(window));
        this_->onCursorPosition(xpos, ypos);
    }

    void mouseButtonCallback(GLFWwindow* window, const int button, const int action, const int mods)
    {
        auto* const this_ = static_cast<Application*>(glfwGetWindowUserPointer(window));
        this_->onMouseButton(button, action, mods);
    }

    void scrollCallback(GLFWwindow* window, const double xoffset, const double yoffset)
    {
        auto* const this_ = static_cast<Application*>(glfwGetWindowUserPointer(window));
        this_->onScroll(xoffset, yoffset);
    }
}

GLFWwindow* InputSystem::window = nullptr;
Application* InputSystem::app = nullptr;

void InputSystem::initialize(Application* app, GLFWwindow* window)
{
    InputSystem::app = app;
    InputSystem::window = window;

    glfwSetWindowUserPointer(window, app);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);
}

glm::vec2 InputSystem::getCursorPos()
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return glm::vec2(xpos, ypos);
}

int InputSystem::getKey(int key)
{
    return glfwGetKey(window, key);
}
