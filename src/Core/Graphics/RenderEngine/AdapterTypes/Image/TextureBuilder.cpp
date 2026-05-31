#include "TextureBuilder.h"

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "Device.h"
#include "Texture.h"
#include "TextureRegistry.h"

namespace Graphics {

TextureBuilder::TextureBuilder(VkFormat format, uint32_t width, uint32_t height)
    : image_info(), alloc_info() {
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent = {.width = width, .height = height, .depth = 1};
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.format = format;
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
}

TextureBuilder& TextureBuilder::isShaderResource() {
    image_info.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
    return *this;
}

TextureBuilder& TextureBuilder::isRenderTarget() {
    image_info.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    return *this;
}

TextureBuilder& TextureBuilder::isDepthStencil() {
    image_info.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    return *this;
}

TextureBuilder& TextureBuilder::isCopySource() {
    image_info.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    return *this;
}

TextureBuilder& TextureBuilder::isCopyDestination() {
    image_info.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    return *this;
}

Result<Texture, TextureError> TextureBuilder::create(
    Device& device, TextureRegistry& texture_registry) {
    auto state = device.createTexture(image_info, alloc_info);
    if (state.isError()) return state.getError();

    return texture_registry.addTexture(state.getResult());
}

}  // namespace Graphics
