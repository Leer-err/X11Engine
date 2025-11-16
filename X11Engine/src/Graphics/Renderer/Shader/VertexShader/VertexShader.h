#pragma once

#include <wrl/client.h>

struct ID3D11VertexShader;

class VertexShader {
    friend class VertexShaderBuilder;

   public:
   protected:
    VertexShader(Microsoft::WRL::ComPtr<ID3D11VertexShader> shader);

   private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
};