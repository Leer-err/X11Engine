#include "Dx11IndexBuffer.h"

Dx11IndexBuffer::Dx11IndexBuffer(
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer, size_t size,
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
    : buffer(buffer), context(context), size(size) {}

ID3D11Buffer* Dx11IndexBuffer::get() { return buffer.Get(); }

void Dx11IndexBuffer::update(const char* data, size_t size, size_t offset) {
    D3D11_MAPPED_SUBRESOURCE res;

    HRESULT hr =
        context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, NULL, &res);

    char* dst_ptr = static_cast<char*>(res.pData) + offset;

    memcpy(dst_ptr, data, size);

    context->Unmap(buffer.Get(), 0);
}

uint32_t Dx11IndexBuffer::getIndexCount() { return size / sizeof(uint32_t); }
