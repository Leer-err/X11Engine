#pragma once

#include <array>
#include <cstdint>

struct PostProcessingData {
    std::array<uint32_t, 2> camera_dimensions;
    uint32_t render_target_index;
    uint32_t sampler_index;

    // dithering
    float spread;

    uint32_t color_count;
};