#include "MeshBuilder.h"

#include <cstring>

#include "BufferBuilder.h"
#include "Device.h"
#include "EngineData.h"
#include "Mesh.h"

namespace Graphics {

MeshBuilder::MeshBuilder(const void* vertex_data, size_t vertex_data_size,
                         const void* index_data, size_t index_data_size)
    : vertex_data(vertex_data),
      vertex_data_size(vertex_data_size),
      index_data(index_data),
      index_data_size(index_data_size) {}

Mesh MeshBuilder::create(Device& device, StagingBuffer& staging_buffer) {
    Mesh mesh = {};
    mesh.vertex_buffer = BufferBuilder(vertex_data_size)
                             .isVertexBuffer()
                             .isCopyDestination()
                             .create(device)
                             .getResult();
    mesh.index_buffer = BufferBuilder(index_data_size)
                            .isIndexBuffer()
                            .isCopyDestination()
                            .create(device)
                            .getResult();

    staging_buffer.stageBuffer(mesh.vertex_buffer, vertex_data,
                               vertex_data_size);
    staging_buffer.stageBuffer(mesh.index_buffer, index_data, index_data_size);

    return mesh;
}

}  // namespace Graphics
