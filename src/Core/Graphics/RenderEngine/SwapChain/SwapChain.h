#pragma once

#include <VkBootstrap.h>
#include <vulkan/vulkan.h>

#include "Device.h"
#include "GraphicsConfig.h"
#include "Queue.h"
#include "Semaphore.h"
#include "TextureState.h"

namespace Graphics {

class SwapChain {
   public:
    struct BackBuffer {
        TextureState backbuffer;
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

    TextureState images[SWAP_CHAIN_MAX_SIZE];
    Semaphore semaphores[SWAP_CHAIN_MAX_SIZE];
    uint32_t image_index;
    int swap_chain_size;
};

}  // namespace Graphics