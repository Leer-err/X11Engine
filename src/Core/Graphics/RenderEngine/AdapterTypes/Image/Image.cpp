#include "Image.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace Graphics {

VkImageMemoryBarrier2 Image::createBarrier(VkImageLayout new_layout,
                                           VkPipelineStageFlags2 src_stages,
                                           VkAccessFlags2 src_access,
                                           VkPipelineStageFlags2 dst_stages,
                                           VkAccessFlags2 dst_access,
                                           VkImageAspectFlags aspect) {
    VkImageMemoryBarrier2 barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    barrier.image = image;
    barrier.srcStageMask = src_stages;
    barrier.srcAccessMask = src_access;
    barrier.dstStageMask = dst_stages;
    barrier.dstAccessMask = dst_access;
    barrier.oldLayout = layout;
    barrier.newLayout = new_layout;
    barrier.subresourceRange.aspectMask = aspect;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    layout = new_layout;

    return barrier;
}

}  // namespace Graphics