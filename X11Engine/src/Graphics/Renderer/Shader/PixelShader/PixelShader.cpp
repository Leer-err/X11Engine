#include "PixelShader.h"

PixelShader::PixelShader(Microsoft::WRL::ComPtr<ID3D11PixelShader> shader)
    : shader(shader) {}

Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader::get() const {
    return shader;
}
