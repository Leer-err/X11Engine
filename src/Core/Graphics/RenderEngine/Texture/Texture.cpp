#include "Texture.h"

#include <vulkan/vulkan.h>

namespace Graphics {

Texture::Texture(Device& device, Device::AllocatedImage allocated_image,
                 const VkImageCreateInfo& image_info,
                 const VmaAllocationCreateInfo& alloc_info) {}

VkImageMemoryBarrier2 Texture::createBarrier(VkImageLayout new_layout,
                                             VkPipelineStageFlags2 src_stages,
                                             VkAccessFlags2 src_access,
                                             VkPipelineStageFlags2 dst_stages,
                                             VkAccessFlags2 dst_access) {
    VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT;
    if (format == VK_FORMAT_D24_UNORM_S8_UINT ||
        format == VK_FORMAT_D32_SFLOAT_S8_UINT)
        aspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

    auto barrier = VkImageMemoryBarrier2{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    barrier.image = texture;
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