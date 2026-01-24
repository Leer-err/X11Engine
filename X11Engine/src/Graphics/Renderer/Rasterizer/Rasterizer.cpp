#include "Rasterizer.h"

Microsoft::WRL::ComPtr<ID3D11RasterizerState1>
Engine::Graphics::Rasterizer::get() const {
    return rasterizer;
}