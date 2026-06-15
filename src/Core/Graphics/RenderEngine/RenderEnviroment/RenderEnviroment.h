#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>

namespace Graphics {

struct RenderEnviroment {
    uint32_t width;
    uint32_t height;

    VkImageView render_target;
    bool clear_render_target;
    VkClearValue render_target_clear_value;

    VkImageView depth_stencil;
    bool clear_depth_stencil;
    VkClearValue depth_stencil_clear_value;
};

}  // namespace Graphics