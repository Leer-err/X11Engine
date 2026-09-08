#include "SwapChain.h"

#include <vulkan/vulkan.h>

#include <cstdint>

#include "Queue.h"
#include "Semaphore.h"
#include "VkBootstrap.h"

namespace Graphics {

SwapChain::SwapChain(Device& device, Queue presentation_queue, uint32_t width,
                     uint32_t height, Config::BufferingMode buffering_mode)
    : queue(presentation_queue) {
    swap_chain_size = 2;
    VkPresentModeKHR mode;
    switch (buffering_mode) {
        case Config::BufferingMode::NoBuffering:
            mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
            break;
        case Config::BufferingMode::VSyncNoBuffering:
            mode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        case Config::BufferingMode::DoubleBuffering:
            mode = VK_PRESENT_MODE_FIFO_KHR;
            break;
        case Config::BufferingMode::TripleBuffering:
            mode = VK_PRESENT_MODE_FIFO_KHR;
            swap_chain_size = 3;
            break;
    }

    VkSurfaceFormatKHR format = {};
    format.format = VK_FORMAT_B8G8R8A8_SRGB;
    format.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

    swap_chain = device.createSwapChain(
        format, mode, swap_chain_size,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);

    auto swap_cahin_images = swap_chain.get_images().value();
    swap_chain_size = swap_cahin_images.size();
    for (int i = 0; i < swap_chain_size; i++) {
        auto& backbuffer = backbuffers[i];

        backbuffer.image = swap_cahin_images[i];
        backbuffer.width = width;
        backbuffer.height = height;
        backbuffer.format = format.format;
        backbuffer.ready_for_present = device.createSemaphore();
    }
}

void SwapChain::destroy() {
    vkb::destroy_swapchain(swap_chain);

    // for (int i = 0; i < swap_chain_size; i++) {
    //     semaphores[i].destroy();
    // }
}

void SwapChain::present() {
    VkPresentInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.pNext = nullptr;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores =
        &backbuffers[image_index].ready_for_present.semaphore;
    info.swapchainCount = 1;
    info.pSwapchains = &swap_chain.swapchain;
    info.pImageIndices = &image_index;
    info.pResults = nullptr;

    vkQueuePresentKHR(queue.queue, &info);
}

SwapChain::BackBuffer SwapChain::getBackbuffer(
    const Semaphore& ready_for_render) {
    vkAcquireNextImageKHR(swap_chain.device, swap_chain, UINT64_MAX,
                          ready_for_render.semaphore, VK_NULL_HANDLE,
                          &image_index);

    return backbuffers[image_index];
}

}  // namespace Graphics