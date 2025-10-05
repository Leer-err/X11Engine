#include "Dx11VertexShader.h"

#include <d3d11.h>
#include <winuser.h>

Dx11VertexShader::Dx11VertexShader(
    Microsoft::WRL::ComPtr<ID3D11VertexShader> shader,
    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout)
    : shader(shader), input_layout(input_layout) {}

ID3D11InputLayout* Dx11VertexShader::getInputLayout() {
    return input_layout.Get();
}

ID3D11VertexShader* Dx11VertexShader::get() { return shader.Get(); }