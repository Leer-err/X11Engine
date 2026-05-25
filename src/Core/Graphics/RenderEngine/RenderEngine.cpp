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
#include "RenderPass.h"
#include "ShaderRegistry.h"
#include "StagingBuffer/StagingBuffer.h"
#include "SwapChain.h"

namespace Graphics {

RenderEngine::RenderEngine(const vkb::Instance& instance,
                           const vkb::Device& device,
                           const Queue& graphics_queue,
                           const Queue& presentation_queue,
                           const VmaAllocator& allocator, VkSurfaceKHR surface)
    : device(device, allocator),
      shader_registry(this->device),
      descriptor_set(this->device, this->device.getDeviceProperties()),
      graphics_queue(graphics_queue),
      presentation_queue(presentation_queue),
      surface(surface),
      frames_in_flight{FrameInFlight(this->device, graphics_queue.index),
                       FrameInFlight(this->device, graphics_queue.index)},
      staging_buffer(this->device),
      frame_in_flight_index(0) {
    reinitWindowDependentResources();

    EngineData data = getEngineData();
    render_pass = std::make_unique<RenderPass>(data);

    auto pool = frames_in_flight[frame_in_flight_index].pool;
    auto trace_cmd = pool.getCommandBuffer();
    trace_ctx = this->device.createTracingContext(graphics_queue, trace_cmd);
    pool.reset();
}

RenderEngine::~RenderEngine() {
    device.waitIdle();

    TracyVkDestroy(trace_ctx);

    swap_chain.destroy();
    vkb::destroy_surface(device.getInstance(), surface);
}

void RenderEngine::render() {
    ZoneScoped;

    waitRenderFinished();

    auto pool = frames_in_flight[frame_in_flight_index].pool;
    auto cmd = pool.getCommandBuffer();

    beginFrame(cmd);

    {
        TracyVkZone(trace_ctx, cmd.buffer, "Staging copy");
        staging_buffer.flush(cmd);
    }

    FrameData data = {.frame_in_flight_index = frame_in_flight_index,
                      .cmd = cmd,
                      .env = render_enviroment,
                      .descriptor_set = descriptor_set,
                      .trace_ctx = trace_ctx};

    render_pass->render(data);

    if (trace_dump_counter++ % 10 == 0) {
        TracyVkCollect(trace_ctx, cmd.buffer);
    }

    endFrame(cmd);

    frame_in_flight_index = (frame_in_flight_index + 1) % MAX_FRAMES_IN_FLIGHT;
}

void RenderEngine::beginFrame(const CommandBuffer& cmd) {
    ZoneScoped;

    cmd.begin();
    prepareRenderTargetForRendering(cmd);
}

void RenderEngine::endFrame(const CommandBuffer& cmd) {
    ZoneScoped;

    auto frame_in_flight = frames_in_flight[frame_in_flight_index];

    auto [backbuffer, ready_for_present] = swap_chain.getBackbuffer(
        frame_in_flight.backbuffer_ready_for_rendering);

    copyToBackbuffer(cmd, render_target_texture, backbuffer);
    prepareBackbufferForPresentation(cmd, backbuffer);

    cmd.end();

    auto command_buffer_info = cmd.submit();
    auto signal = ready_for_present.submit();
    auto wait = frame_in_flight.backbuffer_ready_for_rendering.submit();

    VkSubmitInfo2 submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submit_info.waitSemaphoreInfoCount = 1;
    submit_info.pWaitSemaphoreInfos = &wait;
    submit_info.signalSemaphoreInfoCount = 1;
    submit_info.pSignalSemaphoreInfos = &signal;
    submit_info.commandBufferInfoCount = 1;
    submit_info.pCommandBufferInfos = &command_buffer_info;

    vkQueueSubmit2(graphics_queue.queue, 1, &submit_info,
                   frame_in_flight.finished_processing);

    swap_chain.present();
}

void RenderEngine::reinitWindowDependentResources() {
    auto config = Config::App::get().getGraphicsConfig();

    width = config.render_width;
    height = config.render_height;

    swap_chain = SwapChain(device, presentation_queue, config.render_width,
                           config.render_height, config.buffering_mode);

    render_target_texture =
        ImageBuilder(VK_FORMAT_R8G8B8A8_SRGB, config.render_width,
                     config.render_height)
            .isCopySource()
            .isRenderTarget()
            .create(device)
            .getResult();

    depth_stencil_texture =
        ImageBuilder(VK_FORMAT_D24_UNORM_S8_UINT, config.render_width,
                     config.render_height)
            .isDepthStencil()
            .create(device)
            .getResult();

    render_enviroment = RenderEnviroment{};
    render_enviroment.width = width;
    render_enviroment.height = height;
    render_enviroment.render_target =
        device.createTextureView(render_target_texture);
    render_enviroment.clear_render_target = true;
    render_enviroment.render_target_clear_value =
        VkClearValue{.color = {0, 0, 0, 1}};
    render_enviroment.depth_stencil =
        device.createDepthStencil(depth_stencil_texture);
    render_enviroment.clear_depth_stencil = true;
    render_enviroment.clear_depth = 1;
    render_enviroment.clear_stencil = 0;
}

void RenderEngine::waitRenderFinished() {
    ZoneScoped;
    auto finished_processing =
        frames_in_flight[frame_in_flight_index].finished_processing;

    device.waitFence(finished_processing);
    device.resetFence(finished_processing);
}

void RenderEngine::prepareRenderTargetForRendering(const CommandBuffer& cmd) {
    ZoneScoped;
    VkImageMemoryBarrier2 barriers[2] = {};

    barriers[0] = render_target_texture.createBarrier(
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_2_NONE,
        VK_ACCESS_2_NONE, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT |
            VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);

    barriers[1] = depth_stencil_texture.createBarrier(
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        VK_PIPELINE_STAGE_2_NONE, VK_ACCESS_2_NONE,
        VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
            VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
        VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);

    cmd.barrier(barriers, 2, nullptr, 0);
}

void RenderEngine::copyToBackbuffer(const CommandBuffer& cmd,
                                    Image& render_target, Image& backbuffer) {
    ZoneScoped;
    VkImageMemoryBarrier2 barriers[2] = {};
    barriers[0] = backbuffer.createBarrier(
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_2_NONE,
        VK_ACCESS_2_NONE, VK_PIPELINE_STAGE_2_BLIT_BIT,
        VK_ACCESS_2_TRANSFER_WRITE_BIT);
    barriers[1] = render_target.createBarrier(
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, VK_PIPELINE_STAGE_2_BLIT_BIT,
        VK_ACCESS_2_TRANSFER_READ_BIT);

    cmd.barrier(barriers, 2, nullptr, 0);
    cmd.blit(render_target, backbuffer);
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
    // TODO: image lifetime tracking

    auto image = ImageBuilder(VK_FORMAT_R8G8B8A8_SRGB, width, height)
                     .isShaderResource()
                     .isCopyDestination()
                     .create(device)
                     .getResult();

    auto view = device.createTextureView(image);
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
                             .create(device)
                             .getResult();
    mesh.index_buffer = BufferBuilder(index_data_size)
                            .isIndexBuffer()
                            .isCopyDestination()
                            .create(device)
                            .getResult();

    auto handle = mesh_registry.addMesh(mesh);

    staging_buffer.stageBuffer(mesh.vertex_buffer, vertex_data,
                               vertex_data_size);
    staging_buffer.stageBuffer(mesh.index_buffer, index_data, index_data_size);

    return handle;
}

EngineData RenderEngine::getEngineData() {
    return EngineData{device, descriptor_set, shader_registry, mesh_registry,
                      staging_buffer};
}

uint32_t RenderEngine::getWidth() const { return width; }

uint32_t RenderEngine::getHeight() const { return height; }

}  // namespace Graphics