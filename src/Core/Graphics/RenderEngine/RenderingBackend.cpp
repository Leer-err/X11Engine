#include "RenderingBackend.h"

#include <vulkan/vulkan_core.h>

#include <array>

#include "Device.h"
#include "GraphicsConfig.h"

namespace Graphics {

RenderingBackend::RenderingBackend(const vkb::Instance& instance,
                                   const vkb::Device& device,
                                   const Queue& graphics_queue,
                                   const Queue& presentation_queue,
                                   const VmaAllocator& allocator,
                                   VkSurfaceKHR surface)
    : device(instance, device, allocator),
      graphics_queue(graphics_queue),
      presentation_queue(presentation_queue),
      surface(surface),
      frames_in_flight{FrameInFlight(this->device, graphics_queue.index),
                       FrameInFlight(this->device, graphics_queue.index)},
      frame_in_flight_index(0) {
    auto pool = frames_in_flight[frame_in_flight_index].pool;
    auto trace_cmd = pool.getCommandBuffer();
    trace_ctx = this->device.createTracingContext(graphics_queue, trace_cmd);
    pool.reset();

    createSwapChain();
}

RenderingBackend::~RenderingBackend() {
    device.waitIdle();

    TracyVkDestroy(trace_ctx);

    swap_chain.destroy();
    vkb::destroy_surface(device.getInstance(), surface);
}

FrameData RenderingBackend::beginFrame() {
    auto frame_in_flight = frames_in_flight[frame_in_flight_index];

    // Wait for previous frame render finished
    device.waitFence(frame_in_flight.finished_processing);
    device.resetFence(frame_in_flight.finished_processing);

    auto command_buffer = frame_in_flight.pool.getCommandBuffer();
    command_buffer.begin();

    return FrameData{.frame_in_flight_index = frame_in_flight_index,
                     .cmd = command_buffer};
}

void RenderingBackend::endFrame(Texture& rendered_image) {
    auto frame_in_flight = frames_in_flight[frame_in_flight_index];
    auto command_buffer = frame_in_flight.pool.getCommandBuffer();

    // Collect trace info
    if (trace_dump_counter++ % 10 == 0) {
        TracyVkCollect(trace_ctx, command_buffer.buffer);
    }

    // Copy render result to swap chain
    auto [backbuffer, ready_for_present] = swap_chain.getBackbuffer(
        frame_in_flight.backbuffer_ready_for_rendering);
    copyToBackbuffer(command_buffer, rendered_image, backbuffer);
    prepareBackbufferForPresentation(command_buffer, backbuffer);

    command_buffer.end();

    // Submit command buffer
    auto command_buffer_info = command_buffer.submit();
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

    frame_in_flight_index = (frame_in_flight_index + 1) % MAX_FRAMES_IN_FLIGHT;
}

void RenderingBackend::copyToBackbuffer(const CommandBuffer& cmd,
                                        Texture& render_target,
                                        TextureState& backbuffer) {
    ZoneScoped;
    std::array<VkImageMemoryBarrier2, 2> barriers = {};
    barriers[0] = VkImageMemoryBarrier2{};
    barriers[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    barriers[0].image = backbuffer.texture;
    barriers[0].srcStageMask = VK_PIPELINE_STAGE_2_NONE;
    barriers[0].srcAccessMask = VK_ACCESS_2_NONE;
    barriers[0].dstStageMask = VK_PIPELINE_STAGE_2_BLIT_BIT;
    barriers[0].dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    barriers[0].oldLayout = backbuffer.layout;
    barriers[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barriers[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barriers[0].subresourceRange.baseMipLevel = 0;
    barriers[0].subresourceRange.levelCount = 1;
    barriers[0].subresourceRange.baseArrayLayer = 0;
    barriers[0].subresourceRange.layerCount = 1;

    barriers[1] = render_target.createBarrier(
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, VK_PIPELINE_STAGE_2_BLIT_BIT,
        VK_ACCESS_2_TRANSFER_READ_BIT);

    cmd.barrier(barriers);

    backbuffer.layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    cmd.blit(render_target.getState(), backbuffer);
}

void RenderingBackend::prepareBackbufferForPresentation(
    const CommandBuffer& cmd, const TextureState& backbuffer) {
    ZoneScoped;

    auto render_finished = VkImageMemoryBarrier2{};
    render_finished.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    render_finished.image = backbuffer.texture;
    render_finished.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
    render_finished.srcAccessMask = VK_ACCESS_2_NONE;
    render_finished.dstStageMask = VK_PIPELINE_STAGE_2_BLIT_BIT;
    render_finished.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    render_finished.oldLayout = backbuffer.layout;
    render_finished.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    render_finished.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    render_finished.subresourceRange.baseMipLevel = 0;
    render_finished.subresourceRange.levelCount = 1;
    render_finished.subresourceRange.baseArrayLayer = 0;
    render_finished.subresourceRange.layerCount = 1;

    cmd.barrier(&render_finished, 1, nullptr, 0);
}

void RenderingBackend::createSwapChain() {
    swap_chain = SwapChain(device, presentation_queue, 1280, 720,
                           Config::BufferingMode::NoBuffering);
}

Device& RenderingBackend::getDevice() { return device; }

}  // namespace Graphics