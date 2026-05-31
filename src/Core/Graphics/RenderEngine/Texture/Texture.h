#pragma once

#include <vulkan/vulkan_core.h>

#include "TextureHandle.h"
#include "TextureState.h"

namespace Graphics {

class TextureRegistry;

class Texture {
   public:
    Texture();
    Texture(TextureHandle handle, TextureState* texture,
            TextureRegistry* registry);

    TextureHandle getHandle() const;
    TextureState getState() const;

    VkImageMemoryBarrier2 createBarrier(VkImageLayout new_layout,
                                        VkPipelineStageFlags2 src_stages,
                                        VkAccessFlags2 src_access,
                                        VkPipelineStageFlags2 dst_stages,
                                        VkAccessFlags2 dst_access);

   private:
    TextureHandle handle;
    TextureState* image;
    TextureRegistry* registry;
};

}  // namespace Graphics