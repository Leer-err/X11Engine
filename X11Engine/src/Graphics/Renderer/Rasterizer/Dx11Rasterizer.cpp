#include "Dx11Rasterizer.h"

Dx11Rasterizer::Dx11Rasterizer(
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> state)
    : state(state) {}

ID3D11RasterizerState* Dx11Rasterizer::get() { return state.Get(); }
