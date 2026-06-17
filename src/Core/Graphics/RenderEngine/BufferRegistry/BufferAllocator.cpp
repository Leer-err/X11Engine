#include "BufferAllocator.h"

#include <vk_mem_alloc.h>

#include <optional>

namespace Graphics {

BufferAllocator::BufferAllocator(VkDevice device, VmaAllocator allocator)
    : allocator(allocator), device(device) {}

BufferAllocator::~BufferAllocator() {
    for (const auto& [handle, state] : buffers) {
        vmaDestroyBuffer(allocator, state.buffer, state.allocation);
    }
}

std::optional<RawBufferHandle> BufferAllocator::createBuffer(
    const VkBufferCreateInfo& buffer_info,
    const VmaAllocationCreateInfo& alloc_info) {
    VkBuffer buffer = VK_NULL_HANDLE;
    VmaAllocation allocation = nullptr;
    VmaAllocationInfo info = {};

    VkResult result = vmaCreateBuffer(allocator, &buffer_info, &alloc_info,
                                      &buffer, &allocation, &info);

    if (result != VK_SUCCESS) return {};

    RawBufferHandle handle = next_handle++;

    BufferState state;
    state.buffer = buffer;
    state.allocation = allocation;
    state.size = buffer_info.size;

    if ((buffer_info.usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) != 0) {
        VkBufferDeviceAddressInfo address_info = {};
        address_info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        address_info.buffer = state.buffer;
        state.device_address = vkGetBufferDeviceAddress(device, &address_info);
    }

    if ((alloc_info.flags & VMA_ALLOCATION_CREATE_MAPPED_BIT) != 0)
        state.mapped_address = std::bit_cast<uint8_t*>(info.pMappedData);

    buffers.try_emplace(handle, state);

    return handle;
}

std::optional<BufferState> BufferAllocator::getBufferState(
    RawBufferHandle handle) const {
    auto it = buffers.find(handle);

    if (it == buffers.end()) return {};

    return it->second;
}

void BufferAllocator::deleteBuffer(RawBufferHandle handle) {
    auto it = buffers.find(handle);
    if (it == buffers.end()) return;

    const auto& state = it->second;
    vmaDestroyBuffer(allocator, state.buffer, state.allocation);

    buffers.erase(it);
}

}  // namespace Graphics