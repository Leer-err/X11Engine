#pragma once

#include <cstddef>

#include "Buffer.h"
#include "Result.h"

enum class BufferError { NoDataForImmutableResource, WriteFromGPUAndCPU };

class BufferBuilder {
   public:
    BufferBuilder(size_t size);

    BufferBuilder& withData(const void* data);
    BufferBuilder& isShaderResource();
    BufferBuilder& isVertexBuffer(size_t stride, size_t offset = 0);
    BufferBuilder& isIndexBuffer();
    BufferBuilder& isConstantBuffer();
    BufferBuilder& isCPUWritable();
    BufferBuilder& isGPUWritable();

    Result<Buffer, BufferError> create();

   private:
    const void* data;
    size_t size;
    size_t stride;
    size_t offset;

    bool shader_resource;
    bool vertex_buffer;
    bool index_buffer;
    bool constant_buffer;
    bool cpu_writable;
    bool gpu_writable;
};