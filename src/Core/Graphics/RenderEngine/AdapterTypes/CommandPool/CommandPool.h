#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "CommandBuffer.h"
#include "Device.h"

namespace Graphics {

class CommandPool {
   public:
    CommandPool(Device& device, uint32_t queue_index);

    CommandBuffer getCommandBuffer();

    void reset();

   private:
    CommandBuffer createCommandBuffer();

    Device& device;

    VkCommandPool pool;
    CommandBuffer command_buffer;
};

}  // namespace Graphics