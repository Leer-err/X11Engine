#include "Buffer.h"

#include <cstring>

#include "BufferRegistry.h"

namespace Graphics {

Buffer::Buffer(BufferRegistry* registry, BufferHandle handle)
    : handle(handle), registry(registry) {}

void Buffer::update(const uint8_t* data, size_t size, size_t offset) {
    memcpy(getHostAddress() + offset, data, size);
}

uint8_t* Buffer::getHostAddress() const { return getState().mapped_address; }

VkDeviceAddress Buffer::getDeviceAddress() const {
    return getState().device_address;
}

BufferState Buffer::getState() const { return registry->getState(handle); }

VkBufferMemoryBarrier2 Buffer::createBarrier(VkPipelineStageFlags2 src_stages,
                                             VkAccessFlags2 src_access,
                                             VkPipelineStageFlags2 dst_stages,
                                             VkAccessFlags2 dst_access) {
    VkBufferMemoryBarrier2 barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
    barrier.buffer = getState().buffer;
    barrier.srcStageMask = src_stages;
    barrier.srcAccessMask = src_access;
    barrier.dstStageMask = dst_stages;
    barrier.dstAccessMask = dst_access;
    barrier.size = VK_WHOLE_SIZE;

    return barrier;
}

}  // namespace Graphics