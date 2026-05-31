#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>

#include "Device.h"
#include "Result.h"
#include "Texture.h"

namespace Graphics {

class TextureBuilder {
   public:
    TextureBuilder(VkFormat format, uint32_t width, uint32_t height);

    TextureBuilder& isShaderResource();
    TextureBuilder& isRenderTarget();
    TextureBuilder& isDepthStencil();
    TextureBuilder& isCopySource();
    TextureBuilder& isCopyDestination();

    Result<Texture, TextureError> create(Device& device,
                                         TextureRegistry& texture_registry);

   private:
    VkImageCreateInfo image_info;
    VmaAllocationCreateInfo alloc_info;
};

}  // namespace Graphics