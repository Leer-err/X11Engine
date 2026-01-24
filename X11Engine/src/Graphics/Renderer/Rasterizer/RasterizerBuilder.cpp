#include "RasterizerBuilder.h"

#include <d3d11.h>

#include "APIResources.h"
#include "Rasterizer.h"

Engine::Graphics::Rasterizer Engine::Graphics::RasterizerBuilder::create() {
    D3D11_RASTERIZER_DESC1 desc = {};
    desc.FrontCounterClockwise = true;
    desc.DepthBias = 0;
    desc.SlopeScaledDepthBias = 0.0f;
    desc.DepthBiasClamp = 0.0f;
    desc.DepthClipEnable = TRUE;
    desc.ScissorEnable = FALSE;
    desc.MultisampleEnable = FALSE;
    desc.AntialiasedLineEnable = FALSE;
    desc.ForcedSampleCount = 0;

    switch (fill) {
        case Engine::Graphics::Fill::Solid:
            desc.FillMode = D3D11_FILL_SOLID;
            break;
        case Engine::Graphics::Fill::Wireframe:
            desc.FillMode = D3D11_FILL_WIREFRAME;
            break;
    }

    switch (cull) {
        case Engine::Graphics::Cull::Front:
            desc.CullMode = D3D11_CULL_FRONT;
            break;
        case Engine::Graphics::Cull::Back:
            desc.CullMode = D3D11_CULL_BACK;
            break;
        case Engine::Graphics::Cull::None:
            desc.CullMode = D3D11_CULL_NONE;
            break;
    }

    auto device = APIResources::get().getDevice();

    Microsoft::WRL::ComPtr<ID3D11RasterizerState1> rasterizer;
    HRESULT result = device->CreateRasterizerState1(&desc, &rasterizer);

    return Rasterizer(rasterizer);
}

Engine::Graphics::RasterizerBuilder&
Engine::Graphics::RasterizerBuilder::fillMode(Fill fill_mode) {
    fill = fill_mode;

    return *this;
}

Engine::Graphics::RasterizerBuilder&
Engine::Graphics::RasterizerBuilder::cullMode(Cull cull_mode) {
    cull = cull_mode;

    return *this;
}