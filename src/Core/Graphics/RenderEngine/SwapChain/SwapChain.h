#pragma once

#include <VkBootstrap.h>
#include <vulkan/vulkan.h>

#include <array>

#include "Device.h"
#include "GraphicsConfig.h"
#include "Queue.h"
#include "Semaphore.h"
#include "TextureState.h"

namespace Graphics {

class SwapChain {
   public:
    struct BackBuffer {
        VkImage image;
        VkFormat format;
        uint32_t width;
        uint32_t height;

        Semaphore ready_for_present;
    };

    SwapChain() = default;
    SwapChain(Device& device, Queue presentation_queue, uint32_t width,
              uint32_t height, Config::BufferingMode buffering_mode);

    void destroy();

    void present();

    BackBuffer getBackbuffer(const Semaphore& ready_for_render);

   private:
    static constexpr size_t SWAP_CHAIN_MAX_SIZE = 3;

    Queue queue;

    vkb::Swapchain swap_chain;

    std::array<BackBuffer, SWAP_CHAIN_MAX_SIZE> backbuffers;
    uint32_t image_index;
    int swap_chain_size;
};

}  // namespace Graphics