#pragma once

#include "CommandPool.h"
#include "Device.h"
#include "EngineConstants.h"
#include "EngineData.h"
#include "FrameData.h"
#include "SwapChain.h"

namespace Graphics {

class RenderingBackend {
    struct FrameInFlight {
        FrameInFlight(Device& device, uint32_t queue_index)
            : pool(device, queue_index),
              backbuffer_ready_for_rendering(device.createSemaphore()),
              finished_processing(device.createFence(true)) {}

        CommandPool pool;

        Semaphore backbuffer_ready_for_rendering;
        VkFence finished_processing;
    };

   public:
    RenderingBackend(const vkb::Instance& instance, const vkb::Device& device,
                     const Queue& graphics_queue,
                     const Queue& presentation_queue,
                     const VmaAllocator& allocator, VkSurfaceKHR surface);
    ~RenderingBackend();

    FrameData beginFrame();
    void endFrame();

    Device& getDevice();

   private:
    void copyToBackbuffer(const CommandBuffer& cmd, Image& render_target,
                          Image& backbuffer);
    void prepareBackbufferForPresentation(const CommandBuffer& cmd,
                                          Image& backbuffer);

    void createSwapChain();

    Device device;
    Queue graphics_queue;
    Queue presentation_queue;
    VkSurfaceKHR surface;

    SwapChain swap_chain;

    TracyVkCtx trace_ctx;
    int trace_dump_counter = 0;

    std::array<FrameInFlight, MAX_FRAMES_IN_FLIGHT> frames_in_flight;
    uint32_t frame_in_flight_index;
};

}  // namespace Graphics