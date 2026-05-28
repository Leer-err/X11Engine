#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>

#include "Device.h"
#include "EngineData.h"
#include "Image.h"
#include "Result.h"

namespace Graphics {

class ImageBuilder {
   public:
    ImageBuilder(VkFormat format, uint32_t width, uint32_t height);

    ImageBuilder& isShaderResource();
    ImageBuilder& isRenderTarget();
    ImageBuilder& isDepthStencil();
    ImageBuilder& isCopySource();
    ImageBuilder& isCopyDestination();

    Result<Image, ImageError> create(Device& device);

   private:
    VkImageCreateInfo image_info;
    VmaAllocationCreateInfo alloc_info;
};

}  // namespace Graphics