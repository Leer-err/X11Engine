#include "Sampler.h"

Microsoft::WRL::ComPtr<ID3D11SamplerState> Sampler::get() const {
    return sampler;
}