#pragma once

#include <vulkan/vulkan_core.h>

#include "TextureHandle.h"
#include "TextureRegistry.h"

namespace Graphics {

class Texture {
   public:
    Texture();
    Texture(TextureHandle handle, TextureRegistry* registry);

    TextureHandle getHandle() const;
    Image& getImage();

    VkImageMemoryBarrier2 createBarrier(VkImageLayout new_layout,
                                        VkPipelineStageFlags2 src_stages,
                                        VkAccessFlags2 src_access,
                                        VkPipelineStageFlags2 dst_stages,
                                        VkAccessFlags2 dst_access);

   private:
    TextureHandle handle;
    TextureRegistry* registry;
};

}  // namespace Graphics