#include "RenderEngine.h"

#include <VkBootstrap.h>
#include <vulkan/vulkan.h>

#include <tracy/Tracy.hpp>
#include <tracy/TracyVulkan.hpp>

#include "DescriptorSet.h"
#include "EngineData.h"
#include "FrameData.h"
#include "MeshBuilder.h"
#include "MeshRegistry.h"
#include "StagingBuffer.h"
#include "TextureBuilder.h"

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
      texture_registry(this->backend.getDevice()),
      staging_buffer(this->backend.getDevice()) {
    EngineData data = getEngineData();
    render_pass = std::make_unique<RenderPass>(this->backend.getDevice(), data);
}

void RenderEngine::render() {
    ZoneScoped;

    FrameData frame = backend.beginFrame();

    staging_buffer.flush(frame.cmd);
    auto rendered_image = render_pass->render(frame, world);

    backend.endFrame(rendered_image);
}

TextureHandle RenderEngine::addTexture(void* data, uint32_t width,
                                       uint32_t height) {
    if (data == nullptr) return {};

    auto image = TextureBuilder(VK_FORMAT_R8G8B8A8_SRGB, width, height)
                     .isShaderResource()
                     .isCopyDestination()
                     .create(backend.getDevice(), texture_registry)
                     .getResult();

    auto handle = descriptor_set.addTexture(image);

    staging_buffer.stageTexture(image, data, width * height * 4);

    return handle;
}

MeshHandle RenderEngine::addMesh(void* vertex_data, size_t vertex_data_size,
                                 void* index_data, size_t index_data_size) {
    auto mesh =
        MeshBuilder(vertex_data, vertex_data_size, index_data, index_data_size)
            .create(backend.getDevice(), staging_buffer);

    auto handle = mesh_registry.addMesh(mesh);

    return handle;
}

EngineData RenderEngine::getEngineData() {
    return EngineData{descriptor_set, shader_registry, mesh_registry,
                      texture_registry, staging_buffer};
}

RenderWorld& RenderEngine::getRenderWorld() { return world; }

}  // namespace Graphics