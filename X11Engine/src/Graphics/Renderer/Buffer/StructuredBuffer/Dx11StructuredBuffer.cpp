#include "Dx11StructuredBuffer.h"

#include <wrl/client.h>

Dx11StructuredBuffer::Dx11StructuredBuffer(
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer,
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view,
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
    : buffer(buffer), context(context), view(view) {}

ID3D11Buffer* Dx11StructuredBuffer::get() { return buffer.Get(); }

ID3D11ShaderResourceView* Dx11StructuredBuffer::getView() { return view.Get(); }

void Dx11StructuredBuffer::update(const char* data, size_t size,
                                  size_t offset) {
    D3D11_MAPPED_SUBRESOURCE res;

    HRESULT hr =
        context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, NULL, &res);

    char* dst_ptr = static_cast<char*>(res.pData) + offset;

    memcpy(dst_ptr, data, size);

    context->Unmap(buffer.Get(), 0);
}