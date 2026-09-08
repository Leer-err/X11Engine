#pragma once

#include "DescriptorSet.h"
#include "Registries.h"

namespace Graphics {

class TextureBuilder {
   public:
    TextureBuilder(VkFormat format, uint32_t width, uint32_t height);

    TextureBuilder& setName(std::string_view name);
    TextureBuilder& isShaderResource();
    TextureBuilder& isRenderTarget();
    TextureBuilder& isDepthStencil();
    TextureBuilder& isCopySource();
    TextureBuilder& isCopyDestination();

    Result<TextureHandle, TextureError> create(
        Device& device, TextureRegistry& texture_registry,
        DescriptorSet& descriptor_set);

   private:
    VkImageCreateInfo image_info;
    VmaAllocationCreateInfo alloc_info;

    std::string name;
};

}  // namespace Graphics