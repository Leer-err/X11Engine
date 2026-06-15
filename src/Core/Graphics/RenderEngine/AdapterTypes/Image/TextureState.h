#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <cstddef>

namespace Graphics {

enum class TextureError {};

struct TextureState {
    VkImage texture;
    VmaAllocation allocation;
    VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;

    VkImageView view;

    VkFormat format;
    size_t width;
    size_t height;
};

}  // namespace Graphics