#include "VertexShader.h"

VertexShader::VertexShader(Microsoft::WRL::ComPtr<ID3D11VertexShader> shader,
                           Microsoft::WRL::ComPtr<ID3DBlob> bytecode)
    : shader(shader), bytecode(bytecode) {}

Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader::get() const {
    return shader;
}

Microsoft::WRL::ComPtr<ID3DBlob> VertexShader::getBytecode() const {
    return bytecode;
}