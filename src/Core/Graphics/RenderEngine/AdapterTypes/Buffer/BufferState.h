#pragma once

#include <stddef.h>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace Graphics {

enum class BufferError { NoDataForImmutableResource, WriteFromGPUAndCPU };

struct BufferState {
    VkBuffer buffer;
    VmaAllocation allocation;

    uint8_t* mapped_address;
    VkDeviceAddress device_address;
    size_t size;
};

}  // namespace Graphics
