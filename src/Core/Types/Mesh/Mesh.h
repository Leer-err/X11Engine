#pragma once

#include <cstdint>
#include <vector>

#include "VertexFormats.h"

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};