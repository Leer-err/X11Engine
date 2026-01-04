#include "SamplerBuilder.h"

#include <d3d11.h>
#include <winnt.h>

#include "APIResources.h"
#include "Sampler.h"

Sampler SamplerBuilder::create() {
    D3D11_SAMPLER_DESC desc = {};
    switch (filter) {
        case Filter::Point:
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            break;
        case Filter::Linear:
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            break;
        case Filter::Anisotropic:
            desc.Filter = D3D11_FILTER_ANISOTROPIC;
    }

    desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

    auto device = APIResources::get().getDevice();

    Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
    HRESULT result = device->CreateSamplerState(&desc, &sampler);

    return Sampler(sampler);
}