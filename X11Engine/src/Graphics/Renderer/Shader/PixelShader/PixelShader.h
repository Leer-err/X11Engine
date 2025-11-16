#pragma once

#include <wrl/client.h>

struct ID3D11PixelShader;

class PixelShader {
    friend class PixelShaderBuilder;

   public:
   protected:
    PixelShader(Microsoft::WRL::ComPtr<ID3D11PixelShader> shader);

   private:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
};