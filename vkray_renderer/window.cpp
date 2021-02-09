#include "window.h"

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
        this_->onCursorPosition(window, xpos, ypos);
    }

    void mouseButtonCallback(GLFWwindow* window, const int button, const int action, const int mods)
    {
        auto* const this_ = static_cast<Application*>(glfwGetWindowUserPointer(window));
        this_->onMouseButton(window, button, action, mods);
    }

    void scrollCallback(GLFWwindow* window, const double xoffset, const double yoffset)
    {
        auto* const this_ = static_cast<Application*>(glfwGetWindowUserPointer(window));
        this_->onScroll(window, xoffset, yoffset);
    }
}

Window::Window(Application* app)
    : app(app)
{
}

Window::~Window()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::initialize(const int width, const int height, std::string& title)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    glfwSetWindowUserPointer(window, app);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose(window);
}
