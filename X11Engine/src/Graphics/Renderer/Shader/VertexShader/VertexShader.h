#pragma once

#include <d3d11.h>
#include <wrl/client.h>

class VertexShader {
    friend class VertexShaderBuilder;
    friend class InputLayoutBuilder;
    friend class Context;

   public:
    VertexShader() {}

   protected:
    VertexShader(Microsoft::WRL::ComPtr<ID3D11VertexShader> shader,
                 Microsoft::WRL::ComPtr<ID3DBlob> bytecode);

    Microsoft::WRL::ComPtr<ID3D11VertexShader> get() const;
    Microsoft::WRL::ComPtr<ID3DBlob> getBytecode() const;

   private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
    Microsoft::WRL::ComPtr<ID3DBlob> bytecode;
};