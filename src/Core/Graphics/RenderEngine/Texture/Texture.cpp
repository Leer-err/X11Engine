#include "Texture.h"

#include <vulkan/vulkan_core.h>

#include "TextureRegistry.h"

namespace Graphics {

Texture::Texture() : handle(0), registry(nullptr) {}

Texture::Texture(TextureHandle handle, TextureRegistry* registry)
    : handle(handle), registry(registry) {}

TextureHandle Texture::getHandle() const { return handle; }

TextureState Texture::getState() const {
    return *registry->getTextureState(handle);
}

VkImageMemoryBarrier2 Texture::createBarrier(VkImageLayout new_layout,
                                             VkPipelineStageFlags2 src_stages,
                                             VkAccessFlags2 src_access,
                                             VkPipelineStageFlags2 dst_stages,
                                             VkAccessFlags2 dst_access,
                                             VkImageAspectFlags aspect) {
    auto state = registry->getTextureState(handle);

    auto barrier = VkImageMemoryBarrier2{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    barrier.image = state->texture;
    barrier.srcStageMask = src_stages;
    barrier.srcAccessMask = src_access;
    barrier.dstStageMask = dst_stages;
    barrier.dstAccessMask = dst_access;
    barrier.oldLayout = state->layout;
    barrier.newLayout = new_layout;
    barrier.subresourceRange.aspectMask = aspect;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    state->layout = new_layout;

    return barrier;
}

}  // namespace Graphics