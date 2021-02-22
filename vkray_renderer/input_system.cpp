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

        glm::vec2 lastPos = InputSystem::getLastCursorPos();
        InputSystem::setCusorMotion(glm::vec2(xpos - lastPos.x, ypos - lastPos.y));

        InputSystem::setLastCursorPos(glm::vec2(xpos, ypos));

        this_->onCursorPosition(xpos, ypos);
    }

    void mouseButtonCallback(GLFWwindow* window, const int button, const int action, const int mods)
    {
        auto* const this_ = static_cast<Application*>(glfwGetWindowUserPointer(window));

        if (action == GLFW_PRESS) {
            InputSystem::setButtons(button, PressState::PRESSED);
            //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            //std::cout << "PRESSED\n";
            //if (InputSystem::getMouseButtonState(button) == PressState::PRESSED) {
            //    InputSystem::setMouseButtons(button, PressState::HELD);
            //} else {
            //    InputSystem::setMouseButtons(button, PressState::PRESSED);
            //}
            InputSystem::setLastCursorPos(InputSystem::getCursorPos());
        } else {
            InputSystem::setButtons(button, PressState::RELEASED);
            //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            //std::cout << "RELEASED\n";
            //if (InputSystem::getMouseButtonState(button) == PressState::PRESSED
            //    || InputSystem::getMouseButtonState(button) == PressState::HELD) {
            //    InputSystem::setMouseButtons(button, PressState::RELEASED);
            //} else {
            //    InputSystem::setMouseButtons(button, PressState::NONE);
            //}
        }

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
glm::vec2 InputSystem::lastCusorPos;
glm::vec2 InputSystem::cusorMotion;
PressState InputSystem::buttons[10] = { PressState::NONE };

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

glm::vec2 InputSystem::getLastCursorPos()
{
    return InputSystem::lastCusorPos;
}

void InputSystem::setLastCursorPos(glm::vec2 pos)
{
    lastCusorPos = pos;
}

int InputSystem::getKey(int key)
{
    return glfwGetKey(window, key);
}

glm::vec2 InputSystem::getCusorMotion()
{
    return cusorMotion;
}

void InputSystem::setCusorMotion(glm::vec2 motion)
{
    cusorMotion = motion;
}

PressState InputSystem::getButtonState(int button)
{
    return buttons[button];
}

void InputSystem::setButtons(int button, PressState state)
{
    buttons[button] = state;
}
