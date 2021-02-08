#pragma once

#include <string>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

class Application;

class Window
{
public:

    Window(Application* app);

    ~Window();

    void initialize(const int width, const int height, std::string& title);

    bool shouldClose();

    vk::UniqueSurfaceKHR createSurface(vk::Instance instance)
    {
        VkSurfaceKHR _surface;
        if (glfwCreateWindowSurface(VkInstance(instance), window, nullptr, &_surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
        vk::ObjectDestroy<vk::Instance, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> _deleter(instance);
        return vk::UniqueSurfaceKHR(vk::SurfaceKHR(_surface), _deleter);
    }

private:

    Application* app;

    GLFWwindow* window;

    int width;
    int height;

};

