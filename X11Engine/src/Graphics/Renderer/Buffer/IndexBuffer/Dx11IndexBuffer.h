#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <cstddef>

#include "IIndexBuffer.h"

class Dx11IndexBuffer : public IIndexBuffer {
   public:
    Dx11IndexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> buffer, size_t size,
                    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

    ID3D11Buffer* get();

    void update(const char* data, size_t size, size_t offset);

    uint32_t getIndexCount();

   private:
    size_t size;

    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
};