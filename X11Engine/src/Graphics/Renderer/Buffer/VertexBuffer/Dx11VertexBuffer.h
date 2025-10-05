#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "IVertexBuffer.h"

class Dx11VertexBuffer : public IVertexBuffer {
   public:
    Dx11VertexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> buffer,
                     Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

    ID3D11Buffer* get();

    void update(const char* data, size_t size, size_t offset);

   private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
};