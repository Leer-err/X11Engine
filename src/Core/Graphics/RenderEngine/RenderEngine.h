#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>

#include "CommandBuffer.h"
#include "CommandPool.h"
#include "DescriptorSet.h"
#include "Device.h"
#include "DeviceProperties.h"
#include "EngineConstants.h"
#include "EngineData.h"
#include "IRenderEngine.h"
#include "MeshRegistry.h"
// #include "PostProcessingPass.h"
#include "Queue.h"
#include "RenderEnviroment.h"
#include "RenderPass.h"
#include "RenderingBackend.h"
#include "Semaphore.h"
#include "ShaderRegistry.h"
#include "StagingBuffer.h"
#include "StarRenderer.h"
#include "SwapChain.h"
#include "TextureHandle.h"
#include "TextureRegistry.h"
#include "VkBootstrap.h"

namespace Graphics {

class RenderEngine final : public IRenderEngine {
   public:
    RenderEngine(const vkb::Instance& instance, const vkb::Device& device,
                 const Queue& graphics_queue, const Queue& presentation_queue,
                 const VmaAllocator& allocator, VkSurfaceKHR surface);

    void reinitWindowDependentResources();
    void render() override;

    TextureHandle addTexture(void* data, uint32_t width,
                             uint32_t height) override;
    MeshHandle addMesh(void* vertex_data, size_t vertex_data_size,
                       void* index_data, size_t index_data_size) override;

    EngineData getEngineData();

   private:
    void beginFrame(const CommandBuffer& cmd);
    void endFrame(const CommandBuffer& cmd);

    void waitRenderFinished();

    RenderingBackend backend;

    DescriptorSet descriptor_set;
    ShaderRegistry shader_registry;
    MeshRegistry mesh_registry;
    TextureRegistry texture_registry;

    StagingBuffer staging_buffer;

    std::unique_ptr<RenderPass> render_pass;
    // std::unique_ptr<PostProcessingPass> postprocess_pass;
};

}  // namespace Graphics