#include "Dx11DepthStencil.h"

Dx11DepthStencil::Dx11DepthStencil(
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> view,
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> state)
    : depth_stencil(view), state(state) {}

ID3D11DepthStencilView* Dx11DepthStencil::get() { return depth_stencil.Get(); }

ID3D11DepthStencilState* Dx11DepthStencil::getState() { return state.Get(); }
