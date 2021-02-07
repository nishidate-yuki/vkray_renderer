#pragma once

#include "vkray.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "camera.h"

using vkss = vk::ShaderStageFlagBits;
using vkdt = vk::DescriptorType;
using vksgt = vk::RayTracingShaderGroupTypeKHR;

constexpr float WIDTH = 1280;
constexpr float HEIGHT = 720;

struct UniformData
{
    glm::mat4 model;
    glm::mat4 invView;
    glm::mat4 invProj;
};


struct InstanceDataOnDevice
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

    void onCursorPosition(double xpos, double ypos);

    void onMouseButton(int button, int action, int mods);

    void onScroll(double xoffset, double yoffset);

    void createUniformBuffer();

    void updateUniformBuffer();

    void createInstanceDataBuffer(vkr::Model& model);

    void run();

private:

    uint64_t frame{ 0 };

    std::unique_ptr<vkr::Window> window;
    std::unique_ptr<vkr::Instance> instance;
    std::unique_ptr<vkr::Device> device;
    std::unique_ptr<vkr::SwapChain> swapChain;

    std::unique_ptr<vkr::Image> storageImage;

    std::vector<std::unique_ptr<vkr::BottomLevelAccelerationStructure>> blasArray;
    std::unique_ptr<vkr::TopLevelAccelerationStructure> tlas;

    std::unique_ptr<vkr::DescriptorSets> descSets;
    std::unique_ptr<vkr::ShaderManager> shaderManager;

    vk::UniquePipeline pipeline;

    UniformData uniformData;
    std::unique_ptr<vkr::Buffer> ubo;

    // for mouse input
    glm::vec2 lastCursorPos;
    bool nowPressed = false;

    Camera camera;

    std::vector<std::unique_ptr<vkr::Buffer>> instanceDataBuffers;
};
