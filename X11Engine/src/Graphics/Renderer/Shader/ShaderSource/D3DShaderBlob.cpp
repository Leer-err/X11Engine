#include "D3DShaderBlob.h"

D3DShaderBlob::D3DShaderBlob(Microsoft::WRL::ComPtr<ID3DBlob> shader)
    : shader(shader) {}

void* D3DShaderBlob::get() { return shader->GetBufferPointer(); }

size_t D3DShaderBlob::getSize() { return shader->GetBufferSize(); }
