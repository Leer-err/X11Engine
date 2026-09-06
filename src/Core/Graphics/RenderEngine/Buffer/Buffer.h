#pragma once

#include <vulkan/vulkan.h>

#include <bit>
#include <cstddef>
#include <cstdint>
#include <span>

#include "BufferState.h"

namespace Graphics {

using BufferHandle = uint32_t;

class BufferRegistry;
class BufferAllocator;

class Buffer {
   public:
    Buffer(BufferRegistry* registry, BufferAllocator* allocator,
           BufferHandle handle);

    template <typename T>
    void update(const std::span<T> data, size_t offset = 0) {
        update(std::bit_cast<uint8_t*>(data.data()), sizeof(T), offset);
    }
    template <typename T>
    void update(const T& data, size_t offset = 0) {
        update(std::bit_cast<uint8_t*>(&data), sizeof(T), offset);
    }
    void update(const uint8_t* data, size_t size, size_t offset = 0);

    uint8_t* getHostAddress() const;
    VkDeviceAddress getDeviceAddress() const;

    BufferState getState() const;

    VkBufferMemoryBarrier2 createBarrier(VkPipelineStageFlags2 src_stages,
                                         VkAccessFlags2 src_access,
                                         VkPipelineStageFlags2 dst_stages,
                                         VkAccessFlags2 dst_access);

   private:
    BufferHandle handle;
    BufferRegistry* registry;
    BufferAllocator* allocator;
};

}  // namespace Graphics