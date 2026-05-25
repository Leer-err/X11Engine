#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <cstddef>

namespace Graphics {

enum class ImageError {
    UnsupportedFormat,
    NoDataForImmutableResource,
    WriteFromGPUAndCPU
};

struct Image {
    VkImageMemoryBarrier2 createBarrier(
        VkImageLayout new_layout, VkPipelineStageFlags2 src_stages,
        VkAccessFlags2 src_access, VkPipelineStageFlags2 dst_stages,
        VkAccessFlags2 dst_access,
        VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);

    VkImage image;
    VmaAllocation allocation;
    VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;

    VkFormat format;
    size_t width;
    size_t height;
};

}  // namespace Graphics