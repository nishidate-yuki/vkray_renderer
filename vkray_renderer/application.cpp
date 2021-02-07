#include "application.h"

Application::Application()
    : camera{ WIDTH, HEIGHT }
{

}

inline void Application::onCursorPosition(double xpos, double ypos)
{
    if (nowPressed) {
        camera.processMouseMotion(xpos - lastCursorPos.x, ypos - lastCursorPos.y);
        lastCursorPos = glm::vec2(xpos, ypos);
    }
}

inline void Application::onMouseButton(int button, int action, int mods)
{
    if (button == 0) {
        nowPressed = bool(action);
        lastCursorPos = window->getCursorPos();
    }
}

inline void Application::onScroll(double xoffset, double yoffset)
{
    camera.processMouseWheel(float(yoffset));
}

inline void Application::createUniformBuffer()
{
    vk::DeviceSize size = sizeof(UniformData);
    vk::BufferUsageFlags usage{ vk::BufferUsageFlagBits::eUniformBuffer };
    vk::MemoryPropertyFlags prop{ vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent };
    ubo = std::make_unique<vkr::Buffer>(*device, size, usage, prop, &uniformData);

    updateUniformBuffer();
}

inline void Application::updateUniformBuffer()
{
    uniformData.invView = camera.getInvView();
    uniformData.invProj = camera.getInvProj();
    ubo->copy(&uniformData);
}

inline void Application::createInstanceDataBuffer(vkr::Model& model)
{
    const std::vector<vkr::Node>& nodes = model.getNodes();
    const std::vector<vkr::Mesh>& meshes = model.getMeshes();
    const std::vector<vkr::Material>& materials = model.getMaterials();
    const std::vector<vkr::Texture>& textures = model.getTextures();

    vk::DeviceSize size = sizeof(InstanceDataOnDevice);
    vk::BufferUsageFlags usage{ vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst };
    vk::MemoryPropertyFlags prop{ vk::MemoryPropertyFlagBits::eDeviceLocal };

    for (const auto& node : nodes) {
        const auto& mesh = meshes[node.meshIndex];
        const auto& material = materials[mesh.materialIndex];

        InstanceDataOnDevice data;
        data.meshIndex = node.meshIndex;
        data.worldMatrix = node.worldMatrix;
        data.baseColorTextureIndex = material.baseColorTextureIndex;
        data.normalTextureIndex = material.normalTextureIndex;
        data.occlusionTextureIndex = material.occlusionTextureIndex;

        instanceDataBuffers.push_back(
            std::make_unique<vkr::Buffer>(*device, size, usage, prop, &data));
    }

}

void Application::run()
{
    window = std::make_unique<vkr::Window>("vkray", WIDTH, HEIGHT);
    instance = std::make_unique<vkr::Instance>(*window, true);
    device = std::make_unique<vkr::Device>(*instance);
    swapChain = std::make_unique<vkr::SwapChain>(*device, *window);

    window->onCursorPosition = [this](const double xpos, const double ypos) {
        onCursorPosition(xpos, ypos);
    };
    window->onMouseButton = [this](const int button, const int action, const int mods) {
        onMouseButton(button, action, mods);
    };
    window->onScroll = [this](const double xoffset, const double yoffset) {
        onScroll(xoffset, yoffset);
    };

    // Create storage image
    storageImage = swapChain->createStorageImage();

    vkr::Model model;
    model.loadFromFile(*device, "E:/Desktop/quick-vkray/untitled.glb");
    //model.loadFromFile(*device, "E:/3D Objects/glTF-Sample-Models/2.0/DamagedHelmet/glTF/DamagedHelmet.gltf");

    const std::vector<vkr::Node>& nodes = model.getNodes();
    const std::vector<vkr::Mesh>& meshes = model.getMeshes();
    const std::vector<vkr::Material>& materials = model.getMaterials();
    const std::vector<vkr::Texture>& textures = model.getTextures();

    std::cout << "Nodes     : " << nodes.size() << std::endl;
    std::cout << "Meshes    : " << meshes.size() << std::endl;
    std::cout << "Materials : " << materials.size() << std::endl;
    std::cout << "Textures  : " << textures.size() << std::endl;

    // Build BLASs
    for (const auto& mesh : meshes) {
        blasArray.push_back(std::make_unique<vkr::BottomLevelAccelerationStructure>(*device, mesh));
    }

    // Create AS instances
    std::vector<vkr::AccelerationStructureInstance> instances;
    for (const auto& node : nodes) {
        instances.push_back({ static_cast<uint32_t>(node.meshIndex), node.worldMatrix });
    }

    // Create TLAS
    tlas = std::make_unique<vkr::TopLevelAccelerationStructure>(*device, blasArray, instances);

    createInstanceDataBuffer(model);
    createUniformBuffer();

    // Load shaders
    shaderManager = std::make_unique<vkr::ShaderManager>(*device);
    shaderManager->addShader("shaders/raygen.rgen.spv", vkss::eRaygenKHR, "main", vksgt::eGeneral);
    shaderManager->addShader("shaders/miss.rmiss.spv", vkss::eMissKHR, "main", vksgt::eGeneral);
    shaderManager->addShader("shaders/closesthit.rchit.spv", vkss::eClosestHitKHR, "main", vksgt::eTrianglesHitGroup);

    // Create Desc Sets
    descSets = std::make_unique<vkr::DescriptorSets>(*device, 1);
    descSets->addBindging(0, 0, vkdt::eAccelerationStructureKHR, 1, vkss::eRaygenKHR);               // TLAS
    descSets->addBindging(0, 1, vkdt::eStorageImage, 1, vkss::eRaygenKHR);                           // Image
    descSets->addBindging(0, 2, vkdt::eUniformBuffer, 1, vkss::eRaygenKHR);                          // UBO
    descSets->addBindging(0, 3, vkdt::eStorageBuffer, meshes.size(), vkss::eClosestHitKHR);          // Vertex
    descSets->addBindging(0, 4, vkdt::eStorageBuffer, meshes.size(), vkss::eClosestHitKHR);          // Index
    descSets->addBindging(0, 5, vkdt::eCombinedImageSampler, textures.size(), vkss::eClosestHitKHR); // Texture
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
    descSets->addWriteInfo(0, 2, ubo->createDescriptorInfo());
    descSets->addWriteInfo(0, 3, vertexBufferInfo);
    descSets->addWriteInfo(0, 4, indexBufferInfo);
    descSets->addWriteInfo(0, 5, textureInfo);
    descSets->addWriteInfo(0, 6, instanceDataBufferInfo);
    descSets->update();

    // Create Ray Tracing Pipeline
    pipeline = device->createRayTracingPipeline(*descSets, *shaderManager, 1);

    // Init Shader Binding Table
    shaderManager->initShaderBindingTable(*pipeline, 0, 1, 2);

    // Init Draw Command Buffers
    swapChain->initDrawCommandBuffers(*pipeline, *descSets, *shaderManager, *storageImage);

    // Main loop
    while (!window->shouldClose()) {
        ++frame;
        window->pollEvents();
        swapChain->draw();

        updateUniformBuffer();
    }

    device->waitIdle();

}
