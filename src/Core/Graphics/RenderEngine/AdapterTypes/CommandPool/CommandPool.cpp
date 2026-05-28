#include "CommandPool.h"

#include <vulkan/vulkan.h>

#include "Device.h"

namespace Graphics {

CommandPool::CommandPool(Device& device, uint32_t queue_index)
    : device(device) {
    pool = device.createCommandPool(queue_index);
    command_buffer = createCommandBuffer();
}

CommandBuffer CommandPool::getCommandBuffer() { return command_buffer; }

CommandBuffer CommandPool::createCommandBuffer() {
    VkCommandBufferAllocateInfo info = {};
    info.commandPool = pool;
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandBufferCount = 1;

    CommandBuffer buffer = {};
    buffer.buffer = device.createCommandBuffer(pool);

    return buffer;
}

void CommandPool::reset() { device.resetCommandPool(pool); }

}  // namespace Graphics