#include "Dx11ConstantBuffer.h"

#include <d3d11.h>
#include <winnt.h>

#include <cstring>

Dx11ConstantBuffer::Dx11ConstantBuffer(
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer,
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
    : buffer(buffer), context(context) {}

ID3D11Buffer* Dx11ConstantBuffer::get() { return buffer.Get(); }

void Dx11ConstantBuffer::update(const char* data, size_t size, size_t offset) {
    D3D11_MAPPED_SUBRESOURCE res;

    HRESULT hr =
        context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, NULL, &res);

    char* dst_ptr = static_cast<char*>(res.pData) + offset;

    memcpy(dst_ptr, data, size);

    context->Unmap(buffer.Get(), 0);
}
