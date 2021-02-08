#pragma once

#include "vkray.hpp"

#include <GLFW/glfw3.h>

#include "camera.h"

constexpr int WIDTH = 1280;
constexpr int HEIGHT = 720;

struct UniformData
{
    glm::mat4 invView;
    glm::mat4 invProj;
};

struct InstanceData
{
    glm::mat4 worldMatrix{ 1.0f };
    int meshIndex{ -1 };
    int baseColorTextureIndex{ -1 };
    int normalTextureIndex{ -1 };
    int occlusionTextureIndex{ -1 };
};

class Application
{
public:

    ~Application()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void run()
    {
        initWindow();
        initVulkan();
        mainLoop();
    }

    Camera camera;

    glm::vec2 lastCursorPos;

    bool nowPressed = false;

private:

    GLFWwindow* window;

    std::unique_ptr<vkr::Instance> instance;

    vk::UniqueSurfaceKHR surface;

    std::unique_ptr<vkr::Device> device;

    std::unique_ptr<vkr::SwapChain> swapChain;

    std::unique_ptr<vkr::Image> storageImage;

    std::vector<std::unique_ptr<vkr::BottomLevelAccelerationStructure>> blasArray;
    std::unique_ptr<vkr::TopLevelAccelerationStructure> tlas;

    std::unique_ptr<vkr::ShaderManager> shaderManager;

    std::unique_ptr<vkr::DescriptorSets> descSets;

    vk::UniquePipeline pipeline;

    vkr::Model model;

    UniformData uniformData;
    std::unique_ptr<vkr::Buffer> ubo;

    std::vector<std::unique_ptr<vkr::Buffer>> instanceDataBuffers;

    void initWindow();

    void initVulkan();

    void createInstance();

    void createSurface();

    void buildAccelStruct();

    void loadShaders();

    void createDescSets();

    void createUniformBuffer();

    void updateUniformBuffer();

    void createInstanceDataBuffer();

    void mainLoop();
};
