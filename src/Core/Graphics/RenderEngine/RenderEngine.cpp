#include "RenderEngine.h"

#include <VkBootstrap.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <tracy/Tracy.hpp>
#include <tracy/TracyVulkan.hpp>
// #include "AppConfig.h"
// #include "Context.h"
// #include "Format.h"
#include "AppConfig.h"
// #include "Context.h"
#include "BufferBuilder.h"
#include "CommandBuffer.h"
#include "CommandPool.h"
#include "DescriptorSet.h"
#include "DeviceProperties.h"
#include "EngineData.h"
#include "FrameData.h"
#include "ImageBuilder.h"
#include "MeshBuilder.h"
#include "MeshRegistry.h"
#include "RenderPass.h"
#include "RenderingBackend.h"
#include "ShaderRegistry.h"
#include "StagingBuffer.h"
#include "SwapChain.h"
#include "TextureRegistry.h"

namespace Graphics {

RenderEngine::RenderEngine(const vkb::Instance& instance,
                           const vkb::Device& device,
                           const Queue& graphics_queue,
                           const Queue& presentation_queue,
                           const VmaAllocator& allocator, VkSurfaceKHR surface)
    : backend(instance, device, graphics_queue, presentation_queue, allocator,
              surface),
      descriptor_set(this->backend.getDevice(),
                     this->backend.getDevice().getDeviceProperties()),
      shader_registry(this->backend.getDevice()),
      mesh_registry(),
      texture_registry(),
      staging_buffer(this->backend.getDevice()) {
    EngineData data = getEngineData();
    render_pass = std::make_unique<RenderPass>(this->backend.getDevice(), data);
    // postprocess_pass = std::make_unique<PostProcessingPass>(data);
}

void RenderEngine::render() {
    ZoneScoped;

    FrameData frame = backend.beginFrame();

    staging_buffer.flush(frame.cmd);
    render_pass->render(frame);

    backend.endFrame();
}

void RenderEngine::prepareBackbufferForPresentation(const CommandBuffer& cmd,
                                                    Image& backbuffer) {
    ZoneScoped;
    auto render_finished = backbuffer.createBarrier(
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_2_BLIT_BIT,
        VK_ACCESS_2_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_2_NONE,
        VK_ACCESS_2_NONE);

    cmd.barrier(&render_finished, 1, nullptr, 0);
}

TextureHandle RenderEngine::addTexture(void* data, uint32_t width,
                                       uint32_t height) {
    if (data == nullptr) return {};
    // TODO: image lifetime tracking

    auto image = ImageBuilder(VK_FORMAT_R8G8B8A8_SRGB, width, height)
                     .isShaderResource()
                     .isCopyDestination()
                     .create(backend.getDevice())
                     .getResult();

    auto view = backend.getDevice().createTextureView(image);
    auto handle = descriptor_set.addImage(view);

    staging_buffer.stageImage(image, data, width * height * 4);

    return handle;
}

MeshHandle RenderEngine::addMesh(void* vertex_data, size_t vertex_data_size,
                                 void* index_data, size_t index_data_size) {
    Mesh mesh = {};
    mesh.vertex_buffer = BufferBuilder(vertex_data_size)
                             .isVertexBuffer()
                             .isCopyDestination()
                             .create(backend.getDevice())
                             .getResult();
    mesh.index_buffer = BufferBuilder(index_data_size)
                            .isIndexBuffer()
                            .isCopyDestination()
                            .create(backend.getDevice())
                            .getResult();

    auto handle = mesh_registry.addMesh(mesh);

    staging_buffer.stageBuffer(mesh.vertex_buffer, vertex_data,
                               vertex_data_size);
    staging_buffer.stageBuffer(mesh.index_buffer, index_data, index_data_size);

    return handle;
}

EngineData RenderEngine::getEngineData() {
    return EngineData{descriptor_set, shader_registry, mesh_registry,
                      texture_registry, staging_buffer};
}

}  // namespace Graphics