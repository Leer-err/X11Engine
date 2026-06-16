#pragma once

#include "Buffer.h"

namespace Graphics {

struct Mesh {
    Mesh(Buffer vertex_buffer, Buffer meshlet_buffer,
         Buffer meshlet_vertices_buffer, Buffer meshlet_triangles_buffer,
         uint32_t meshlet_count)
        : vertex_buffer(vertex_buffer),
          meshlet_buffer(meshlet_buffer),
          meshlet_vertices_buffer(meshlet_vertices_buffer),
          meshlet_triangles_buffer(meshlet_triangles_buffer),
          meshlet_count(meshlet_count) {}

    Buffer vertex_buffer;

    Buffer meshlet_buffer;
    Buffer meshlet_vertices_buffer;
    Buffer meshlet_triangles_buffer;

    uint32_t meshlet_count;
};

}  // namespace Graphics