#include "Buffer.h"

Buffer::Buffer(Microsoft::WRL::ComPtr<ID3D11Buffer> buffer, size_t size,
               size_t stride, size_t offset)
    : buffer(buffer), size(size), stride(stride), offset(offset) {}

Microsoft::WRL::ComPtr<ID3D11Buffer> Buffer::get() const { return buffer; }

size_t Buffer::getOffset() const { return offset; }

size_t Buffer::getStride() const { return stride; }

size_t Buffer::getSize() const { return size; }
