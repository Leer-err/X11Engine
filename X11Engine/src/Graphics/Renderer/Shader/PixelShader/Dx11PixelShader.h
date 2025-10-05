#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "IPixelShader.h"

class Dx11PixelShader : public IPixelShader {
   public:
    Dx11PixelShader(Microsoft::WRL::ComPtr<ID3D11PixelShader> shader);

    ID3D11PixelShader* get();

   private:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
};