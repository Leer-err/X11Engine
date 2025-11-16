#include "VertexShader.h"

VertexShader::VertexShader(Microsoft::WRL::ComPtr<ID3D11VertexShader> shader)
    : shader(shader) {}