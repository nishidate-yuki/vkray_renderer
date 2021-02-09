#pragma once

#include "vkray.hpp"

#include <GLFW/glfw3.h>

#include "camera.h"
#include "window.h"

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

    Application();

    void run();

    void onCursorPosition(GLFWwindow* window, const double xpos, const double ypos);
    void onMouseButton(GLFWwindow* window, const int button, const int action, const int mods);
    void onScroll(GLFWwindow* window, const double xoffset, const double yoffset);

    Camera camera;

    glm::vec2 lastCursorPos;

    bool nowPressed = false;

private:

    std::string appName;

    Window window;

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

    void initVulkan();

    void createInstance();

    void buildAccelStruct();

    void loadShaders();

    void createDescSets();

    void createUniformBuffer();

    void updateUniformBuffer();

    void createInstanceDataBuffer();

    void mainLoop();
};
