#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "IStructuredBuffer.h"

class Dx11StructuredBuffer : public IStructuredBuffer {
   public:
    Dx11StructuredBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> buffer,
                         Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view,
                         Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

    ID3D11Buffer* get();
    ID3D11ShaderResourceView* getView();

    void update(const char* data, size_t size, size_t offset);

   private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
};