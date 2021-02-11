#pragma once

#include "vkray.hpp"

#include <GLFW/glfw3.h>

#include "camera.h"
#include "window.h"
#include "input_system.h"

constexpr int WIDTH = 1200;
constexpr int HEIGHT = 900;

struct UniformData
{
    glm::mat4 invView;
    glm::mat4 invProj;
    glm::vec3 sunDir;
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
    ~Application();

    void run();

    void onCursorPosition(const double xpos, const double ypos);
    void onMouseButton(const int button, const int action, const int mods);
    void onScroll(const double xoffset, const double yoffset);

    Camera camera;

private:

    std::string appName;

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
