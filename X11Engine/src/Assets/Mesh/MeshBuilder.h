#pragma once

#include <dxgi1_4.h>

#include "Mesh.h"

class MeshBuilder {
   public:
    MeshBuilder() = default;

    MeshBuilder& addVertexData(const void* vertices, size_t vertex_size,
                               size_t data_stride, size_t data_offset = 0);
    MeshBuilder& setIndexData(const void* indices, size_t index_size);

    Mesh create();

   private:
    Buffer vertex_buffer;
    Buffer index_buffer;
};