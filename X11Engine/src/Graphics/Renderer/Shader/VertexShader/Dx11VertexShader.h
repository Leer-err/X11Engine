#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "IVertexShader.h"

class Dx11VertexShader : public IVertexShader {
   public:
    Dx11VertexShader(Microsoft::WRL::ComPtr<ID3D11VertexShader> shader,
                     Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout);

    ID3D11VertexShader* get();
    ID3D11InputLayout* getInputLayout();

   private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
};