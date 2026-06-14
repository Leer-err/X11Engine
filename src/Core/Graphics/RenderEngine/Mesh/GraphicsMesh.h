#pragma once

#include "Buffer.h"

namespace Graphics {

struct Mesh {
    Buffer vertex_buffer;

    Buffer meshlet_buffer;
    Buffer meshlet_vertices_buffer;
    Buffer meshlet_triangles_buffer;

    uint32_t meshlet_count;
};

}  // namespace Graphics