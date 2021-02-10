#include "window.h"

#include "application.h"

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
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose(window);
}
