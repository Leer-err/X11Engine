#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "IConstantBuffer.h"

class Dx11ConstantBuffer : public IConstantBuffer {
   public:
    Dx11ConstantBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> buffer,
                       Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

    void update(const char* data, size_t size, size_t offset) override;

    ID3D11Buffer* get();

   private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
};