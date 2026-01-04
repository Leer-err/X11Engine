#include "BufferBuilder.h"

#include <d3d11.h>

#include "APIResources.h"
#include "Buffer.h"

BufferBuilder::BufferBuilder(size_t size)
    : size(size),
      stride(0),
      offset(0),
      shader_resource(false),
      vertex_buffer(false),
      index_buffer(false),
      constant_buffer(false),
      cpu_writable(false),
      gpu_writable(false),
      data(nullptr) {}

BufferBuilder& BufferBuilder::withData(const void* data) {
    this->data = data;
    return *this;
}

BufferBuilder& BufferBuilder::isShaderResource() {
    shader_resource = true;
    return *this;
}

BufferBuilder& BufferBuilder::isVertexBuffer(size_t stride, size_t offset) {
    this->stride = stride;
    this->offset = offset;

    vertex_buffer = true;
    return *this;
}

BufferBuilder& BufferBuilder::isIndexBuffer() {
    index_buffer = true;
    return *this;
}

BufferBuilder& BufferBuilder::isConstantBuffer() {
    constant_buffer = true;
    return *this;
}

BufferBuilder& BufferBuilder::isCPUWritable() {
    cpu_writable = true;
    return *this;
}

BufferBuilder& BufferBuilder::isGPUWritable() {
    gpu_writable = true;
    return *this;
}

Result<Buffer, BufferError> BufferBuilder::create() {
    // Validation
    if (cpu_writable && gpu_writable) return BufferError::WriteFromGPUAndCPU;
    if (data == nullptr && !gpu_writable && !cpu_writable)
        return BufferError::NoDataForImmutableResource;

    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = size;
    if (shader_resource) desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
    if (constant_buffer) desc.BindFlags |= D3D11_BIND_CONSTANT_BUFFER;
    if (vertex_buffer) desc.BindFlags |= D3D11_BIND_VERTEX_BUFFER;
    if (index_buffer) desc.BindFlags |= D3D11_BIND_INDEX_BUFFER;
    if (gpu_writable) desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

    if (!cpu_writable && !gpu_writable)
        desc.Usage = D3D11_USAGE_IMMUTABLE;
    else if (cpu_writable)
        desc.Usage = D3D11_USAGE_DYNAMIC;
    else if (gpu_writable)
        desc.Usage = D3D11_USAGE_DEFAULT;

    if (cpu_writable) desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA* initial_data = nullptr;

    D3D11_SUBRESOURCE_DATA res = {};
    res.pSysMem = data;
    if (data != nullptr) initial_data = &res;

    auto device = APIResources::get().getDevice();

    Microsoft::WRL::ComPtr<ID3D11Buffer> buf;
    auto result = device->CreateBuffer(&desc, initial_data, buf.GetAddressOf());
    if (FAILED(result)) throw;

    return Buffer(buf, size, stride, offset);
}
