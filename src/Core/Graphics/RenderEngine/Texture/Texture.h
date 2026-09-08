#pragma once

#include <vulkan/vulkan.h>

#include "Descriptors.h"
#include "Device.h"

namespace Graphics {

class Texture {
   public:
    Texture(Device& device, Device::AllocatedImage allocated_image,
            const VkImageCreateInfo& image_info,
            const VmaAllocationCreateInfo& alloc_info);

    VkImageMemoryBarrier2 createBarrier(VkImageLayout new_layout,
                                        VkPipelineStageFlags2 src_stages,
                                        VkAccessFlags2 src_access,
                                        VkPipelineStageFlags2 dst_stages,
                                        VkAccessFlags2 dst_access);

    uint32_t getWidth() const;
    uint32_t getHeight() const;
    VkImage getImage() const;
    VkFormat getFormat() const;
    VkImageLayout getLayout() const;
    VkImageView getView() const;

   private:
    VkImage texture;
    VmaAllocation allocation;
    VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageView view;

    TextureDescriptor descriptor;

    VkFormat format;
    size_t width;
    size_t height;
};

}  // namespace Graphics