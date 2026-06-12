#pragma once

#include <cstdint>
#include <vector>

#include "VertexFormats.h"

struct Meshlet {
    uint32_t vertex_offset;
    uint32_t index_offset;

    uint32_t vertex_count;
    uint32_t index_count;
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    std::vector<Meshlet> meshlets;
};