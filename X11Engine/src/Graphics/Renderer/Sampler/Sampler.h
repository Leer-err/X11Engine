#pragma once

#include <d3d11.h>
#include <wrl/client.h>

class Sampler {
    friend class Context;
    friend class SamplerBuilder;

   public:
    Sampler() = default;

   protected:
    Sampler(Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
        : sampler(sampler) {}

    Microsoft::WRL::ComPtr<ID3D11SamplerState> get() const;

   private:
    Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
};