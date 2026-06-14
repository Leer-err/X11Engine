#pragma once

#include <cstdint>

namespace Graphics {

struct Meshlet {
    uint32_t vertex_offset;
    uint32_t triangle_offset;

    uint8_t vertex_count;
    uint8_t triangle_count;
    uint8_t padding[2];
};

}  // namespace Graphics