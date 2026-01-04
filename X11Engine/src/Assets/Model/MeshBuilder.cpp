#include "MeshBuilder.h"

#include "BufferBuilder.h"

MeshBuilder& MeshBuilder::addVertexData(const void* vertices, size_t data_size,
                                        size_t data_stride,
                                        size_t data_offset) {
    auto vertex_result = BufferBuilder(data_size)
                             .isVertexBuffer(data_stride, data_offset)
                             .withData(vertices)
                             .create();
    if (vertex_result.isError()) throw;

    vertex_buffer = vertex_result.getResult();
    return *this;
}

MeshBuilder& MeshBuilder::setIndexData(const void* indices, size_t data_size) {
    auto index_result =
        BufferBuilder(data_size).isIndexBuffer().withData(indices).create();
    if (index_result.isError()) throw;

    index_buffer = index_result.getResult();
    return *this;
}

Mesh MeshBuilder::create() { return Mesh{vertex_buffer, index_buffer}; }
