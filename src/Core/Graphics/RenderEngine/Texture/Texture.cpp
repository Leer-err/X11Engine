#include "Texture.h"

namespace Graphics {

Texture::Texture() : handle(0), image(nullptr), registry(nullptr) {}

Texture::Texture(TextureHandle handle, TextureState* texture,
                 TextureRegistry* registry)
    : handle(handle), image(texture), registry(registry) {}

TextureHandle Texture::getHandle() const { return handle; }

TextureState Texture::getState() const { return *image; }

VkImageMemoryBarrier2 Texture::createBarrier(VkImageLayout new_layout,
                                             VkPipelineStageFlags2 src_stages,
                                             VkAccessFlags2 src_access,
                                             VkPipelineStageFlags2 dst_stages,
                                             VkAccessFlags2 dst_access) {
    auto barrier = VkImageMemoryBarrier2{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    barrier.image = image->texture;
    barrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
    barrier.srcAccessMask = VK_ACCESS_2_NONE;
    barrier.dstStageMask = VK_PIPELINE_STAGE_2_BLIT_BIT;
    barrier.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    barrier.oldLayout = image->layout;
    barrier.newLayout = new_layout;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    image->layout = new_layout;

    return barrier;
}

}  // namespace Graphics