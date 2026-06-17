#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <optional>
#include <unordered_map>

#include "BufferState.h"

namespace Graphics {

using RawBufferHandle = uint32_t;

class BufferAllocator {
   public:
    explicit BufferAllocator(VkDevice device, VmaAllocator allocator);
    ~BufferAllocator();

    std::optional<RawBufferHandle> createBuffer(
        const VkBufferCreateInfo& buffer_info,
        const VmaAllocationCreateInfo& alloc_info);

    std::optional<BufferState> getBufferState(RawBufferHandle handle) const;

    void deleteBuffer(RawBufferHandle handle);

    BufferAllocator(const BufferAllocator&) = delete;
    BufferAllocator& operator=(const BufferAllocator&) = delete;
    BufferAllocator(BufferAllocator&&) = delete;
    BufferAllocator& operator=(BufferAllocator&&) = delete;

   private:
    VmaAllocator allocator;
    VkDevice device;

    std::unordered_map<RawBufferHandle, BufferState> buffers;

    RawBufferHandle next_handle = 1;
};

}  // namespace Graphics