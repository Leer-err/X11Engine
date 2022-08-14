#pragma once
#include <d3d11_4.h>
#include <stdint.h>
#include <wrl/client.h>

#include <numeric>

using Microsoft::WRL::ComPtr;

constexpr uint32_t INVALID_RESOURCE_INDEX =
    std::numeric_limits<uint32_t>::max();

constexpr uint32_t SHADER_STAGE_COUNT = 5;

constexpr uint32_t VERTEX_SHADER_STAGE = 0;
constexpr uint32_t HULL_SHADER_STAGE = 1;
constexpr uint32_t DOMAIN_SHADER_STAGE = 2;
constexpr uint32_t GEOMETRY_SHADER_STAGE = 3;
constexpr uint32_t PIXEL_SHADER_STAGE = 4;

template <typename T>
struct Shader {
    ComPtr<T> shader;
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

struct ShaderResources {
    ComPtr<ID3D11Resource>
        textures[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];
    ComPtr<ID3D11Buffer>
        constantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT];
    ComPtr<ID3D11SamplerState>
        samplers[D3D11_COMMONSHADER_SAMPLER_REGISTER_COUNT];
};

struct Material {
    PixelShader pixelShader;
    VertexShader vertexShader;

    ShaderResources resources[SHADER_STAGE_COUNT];
};