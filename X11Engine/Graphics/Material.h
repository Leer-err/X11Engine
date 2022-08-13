#pragma once
#include <d3d11_4.h>
#include <stdint.h>
#include <wrl/client.h>

#include <numeric>

using Microsoft::WRL::ComPtr;

constexpr uint32_t INVALID_RESOURCE_INDEX =
    std::numeric_limits<uint32_t>::max();

template <typename T>
struct Shader {
    ComPtr<T> shader;

    ComPtr<ID3D11Resource>
        textures[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];
    ComPtr<ID3D11Buffer>
        constantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT];
    ComPtr<ID3D11SamplerState>
        samplers[D3D11_COMMONSHADER_SAMPLER_REGISTER_COUNT];
};

struct PixelShader : public Shader<ID3D11PixelShader> {
    uint32_t baseColorIndex = INVALID_RESOURCE_INDEX;
    uint32_t specularIndex = INVALID_RESOURCE_INDEX;
    uint32_t emissionIndex = INVALID_RESOURCE_INDEX;
    uint32_t lightsIndex = INVALID_RESOURCE_INDEX;
};

struct VertexShader : public Shader<ID3D11VertexShader> {
    ComPtr<ID3D11InputLayout> inputLayout;
};

struct Material {
    PixelShader pixelShader;
    VertexShader vertexShader;
};