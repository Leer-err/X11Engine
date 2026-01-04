#include "DepthStencil.h"

Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencil::get() const {
    return depth_stencil;
}