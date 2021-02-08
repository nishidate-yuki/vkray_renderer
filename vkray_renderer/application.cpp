#include "application.h"

using vkss = vk::ShaderStageFlagBits;
using vkdt = vk::DescriptorType;
using vksgt = vk::RayTracingShaderGroupTypeKHR;

namespace
{
    void keyCallback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods)
    {
        auto* const this_ = static_cast<Application*>(glfwGetWindowUserPointer(window));
    }

    void cursorPositionCallback(GLFWwindow* window, const double xpos, const double ypos)
    {
        auto* const this_ = static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (this_->nowPressed) {
            this_->camera.processMouseMotion(xpos - this_->lastCursorPos.x, ypos - this_->lastCursorPos.y);
            this_->lastCursorPos = glm::vec2(xpos, ypos);
        }
    }

    void mouseButtonCallback(GLFWwindow* window, const int button, const int action, const int mods)
    {
        auto* const this_ = static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (button == 0) {
            this_->nowPressed = bool(action);
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            this_->lastCursorPos = glm::vec2(xpos, ypos);
        }
    }

    void scrollCallback(GLFWwindow* window, const double xoffset, const double yoffset)
    {
        auto* const this_ = static_cast<Application*>(glfwGetWindowUserPointer(window));
        this_->camera.processMouseWheel(float(yoffset));
    }
}

void Application::initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "quick vkray", nullptr, nullptr);
}

void Application::initVulkan()
{
    createInstance();
    createSurface();
    device = std::make_unique<vkr::Device>(*instance, *surface);
    swapChain = std::make_unique<vkr::SwapChain>(*device, vk::Extent2D{ WIDTH, HEIGHT });

    storageImage = swapChain->createStorageImage();

    buildAccelStruct();

    loadShaders();

    camera = Camera(WIDTH, HEIGHT);

    createUniformBuffer();

    createInstanceDataBuffer();

    createDescSets();

    pipeline = descSets->createRayTracingPipeline(*shaderManager, 1);
    shaderManager->initShaderBindingTable(*pipeline, 0, 1, 2);
    swapChain->initDrawCommandBuffers(*pipeline, *descSets, *shaderManager, *storageImage);

    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);
}

void Application::createInstance()
{
    // Get GLFW extensions
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    vk::ApplicationInfo appInfo;
    appInfo.setPApplicationName("quick vkray");
    appInfo.setApiVersion(VK_API_VERSION_1_2);

    instance = std::make_unique<vkr::Instance>(appInfo, enableValidationLayers, extensions);
}

void Application::createSurface()
{
    VkSurfaceKHR _surface;
    if (glfwCreateWindowSurface(VkInstance(instance->getHandle()), window, nullptr, &_surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
    vk::ObjectDestroy<vk::Instance, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> _deleter(instance->getHandle());
    surface = vk::UniqueSurfaceKHR(vk::SurfaceKHR(_surface), _deleter);
}

void Application::buildAccelStruct()
{
    model.loadFromFile(*device, "assets/Sponza/Sponza.gltf");

    const std::vector<vkr::Node>& nodes = model.getNodes();
    const std::vector<vkr::Mesh>& meshes = model.getMeshes();
    const std::vector<vkr::Material>& materials = model.getMaterials();
    const std::vector<vkr::Texture>& textures = model.getTextures();

    for (const auto& mesh : meshes) {
        blasArray.push_back(std::make_unique<vkr::BottomLevelAccelerationStructure>(*device, mesh));
    }

    std::vector<vkr::AccelerationStructureInstance> instances;
    for (const auto& node : nodes) {
        instances.push_back({ static_cast<uint32_t>(node.meshIndex), node.worldMatrix });
    }

    tlas = std::make_unique<vkr::TopLevelAccelerationStructure>(*device, blasArray, instances);
}

void Application::loadShaders()
{
    shaderManager = std::make_unique<vkr::ShaderManager>(*device);
    shaderManager->addShader("shaders/raygen.rgen.spv", vkss::eRaygenKHR, "main", vksgt::eGeneral);
    shaderManager->addShader("shaders/miss.rmiss.spv", vkss::eMissKHR, "main", vksgt::eGeneral);
    shaderManager->addShader("shaders/closesthit.rchit.spv", vkss::eClosestHitKHR, "main", vksgt::eTrianglesHitGroup);
}

void Application::createDescSets()
{
    const std::vector<vkr::Node>& nodes = model.getNodes();
    const std::vector<vkr::Mesh>& meshes = model.getMeshes();
    const std::vector<vkr::Texture>& textures = model.getTextures();

    descSets = std::make_unique<vkr::DescriptorSets>(*device, 1);
    descSets->addBindging(0, 0, vkdt::eAccelerationStructureKHR, 1, vkss::eRaygenKHR);               // TLAS
    descSets->addBindging(0, 1, vkdt::eStorageImage, 1, vkss::eRaygenKHR);                           // Storage Image
    descSets->addBindging(0, 2, vkdt::eStorageBuffer, meshes.size(), vkss::eClosestHitKHR);          // Vertex
    descSets->addBindging(0, 3, vkdt::eStorageBuffer, meshes.size(), vkss::eClosestHitKHR);          // Index
    descSets->addBindging(0, 4, vkdt::eCombinedImageSampler, textures.size(), vkss::eClosestHitKHR); // Texture
    descSets->addBindging(0, 5, vkdt::eUniformBuffer, 1, vkss::eRaygenKHR);                          // UBO
    descSets->addBindging(0, 6, vkdt::eUniformBuffer, nodes.size(), vkss::eClosestHitKHR);           // Instance data
    descSets->initPipelineLayout();

    // Create DescInfo array
    std::vector<vk::DescriptorBufferInfo> vertexBufferInfo;
    std::vector<vk::DescriptorBufferInfo> indexBufferInfo;
    for (const auto& mesh : meshes) {
        vertexBufferInfo.push_back(mesh.vertexBuffer->createDescriptorInfo());
        indexBufferInfo.push_back(mesh.indexBuffer->createDescriptorInfo());
    }
    std::vector<vk::DescriptorImageInfo> textureInfo;
    for (const auto& tex : textures) {
        textureInfo.push_back(tex.createDescriptorInfo());
    }
    std::vector<vk::DescriptorBufferInfo> instanceDataBufferInfo;
    for (const auto& bufffer : instanceDataBuffers) {
        instanceDataBufferInfo.push_back(bufffer->createDescriptorInfo());
    }

    descSets->allocate();
    descSets->addWriteInfo(0, 0, tlas->createWrite());
    descSets->addWriteInfo(0, 1, storageImage->createDescriptorInfo());
    descSets->addWriteInfo(0, 2, vertexBufferInfo);
    descSets->addWriteInfo(0, 3, indexBufferInfo);
    descSets->addWriteInfo(0, 4, textureInfo);
    descSets->addWriteInfo(0, 5, ubo->createDescriptorInfo());
    descSets->addWriteInfo(0, 6, instanceDataBufferInfo);
    descSets->update();
}

void Application::createUniformBuffer()
{
    vk::DeviceSize size = sizeof(UniformData);
    vk::BufferUsageFlags usage{ vk::BufferUsageFlagBits::eUniformBuffer };
    vk::MemoryPropertyFlags prop{ vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent };
    ubo = std::make_unique<vkr::Buffer>(*device, size, usage, prop, &uniformData);

    updateUniformBuffer();
}

void Application::updateUniformBuffer()
{
    uniformData.invView = glm::inverse(camera.view);
    uniformData.invProj = glm::inverse(camera.proj);
    ubo->copy(&uniformData);
}

void Application::createInstanceDataBuffer()
{
    const std::vector<vkr::Node>& nodes = model.getNodes();
    const std::vector<vkr::Mesh>& meshes = model.getMeshes();
    const std::vector<vkr::Material>& materials = model.getMaterials();
    const std::vector<vkr::Texture>& textures = model.getTextures();

    vk::DeviceSize size = sizeof(InstanceData);
    vk::BufferUsageFlags usage{ vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst };
    vk::MemoryPropertyFlags prop{ vk::MemoryPropertyFlagBits::eDeviceLocal };

    for (const auto& node : nodes) {
        const auto& mesh = meshes[node.meshIndex];
        const auto& material = materials[mesh.materialIndex];

        InstanceData data;
        data.meshIndex = node.meshIndex;
        data.worldMatrix = node.worldMatrix;
        data.baseColorTextureIndex = material.baseColorTextureIndex;
        data.normalTextureIndex = material.normalTextureIndex;
        data.occlusionTextureIndex = material.occlusionTextureIndex;

        instanceDataBuffers.push_back(std::make_unique<vkr::Buffer>(*device, size, usage, prop, &data));
    }
}

void Application::mainLoop()
{
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        swapChain->draw();
        updateUniformBuffer();
    }
    device->waitIdle();
}
