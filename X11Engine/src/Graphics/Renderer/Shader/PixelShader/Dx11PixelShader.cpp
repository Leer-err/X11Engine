#include "Dx11PixelShader.h"

Dx11PixelShader::Dx11PixelShader(
    Microsoft::WRL::ComPtr<ID3D11PixelShader> shader)
    : shader(shader) {}

ID3D11PixelShader* Dx11PixelShader::get() { return shader.Get(); }
