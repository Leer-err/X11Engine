#pragma once

#include <d3d11.h>
#include <wrl/client.h>

class PixelShader {
    friend class PixelShaderBuilder;
    friend class Context;

   public:
    PixelShader() {}

   protected:
    PixelShader(Microsoft::WRL::ComPtr<ID3D11PixelShader> shader);

    Microsoft::WRL::ComPtr<ID3D11PixelShader> get() const;

   private:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
};