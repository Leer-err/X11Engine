#pragma once

#include <cstddef>

#include "Device.h"
#include "GraphicsMesh.h"
#include "Mesh.h"
#include "StagingBuffer.h"

namespace Graphics {

class MeshBuilder {
   public:
    MeshBuilder(const void* vertex_data, size_t vertex_data_size,
                const void* index_data, size_t index_data_size);
    MeshBuilder(const ::Mesh& mesh);

    Mesh create(Device& device, StagingBuffer& staging_buffer);

   private:
    const void* vertex_data;
    size_t vertex_data_size;
    const void* index_data;
    size_t index_data_size;
};

}  // namespace Graphics