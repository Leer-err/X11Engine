#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <string>

#include "BufferRegistry.h"
#include "CommandBuffer.h"
#include "DescriptorSet.h"
#include "Device.h"
#include "EngineData.h"
#include "FrameGraph.h"
#include "IRenderEngine.h"
#include "MeshRegistry.h"
#include "Queue.h"
#include "RenderPass.h"
#include "RenderWorld.h"
#include "RenderingBackend.h"
#include "ShaderRegistry.h"
#include "StagingBuffer.h"
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

    RenderWorld& getRenderWorld() override;

    TextureHandle addTexture(void* data, uint32_t width,
                             uint32_t height) override;
    TextureHandle addTexture(std::string_view name, void* data, uint32_t width,
                             uint32_t height) override;
    MeshHandle addMesh(const ::Mesh& mesh) override;
    MeshHandle addMesh(std::string_view name, const ::Mesh& mesh) override;

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

    RenderWorld world;

    std::unique_ptr<RenderPass> render_pass;
};

}  // namespace Graphics