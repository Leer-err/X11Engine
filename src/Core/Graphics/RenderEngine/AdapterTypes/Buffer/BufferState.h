#pragma once

#include <stddef.h>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace Graphics {

enum class BufferError {};

struct BufferState {
    VkBuffer buffer = VK_NULL_HANDLE;
    VmaAllocation allocation = nullptr;

    uint8_t* mapped_address = nullptr;
    VkDeviceAddress device_address = 0;
    size_t size = 0;
};

}  // namespace Graphics
