#include "Dx11RenderTarget.h"

Dx11RenderTarget::Dx11RenderTarget(
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target)
    : render_target(render_target) {}

ID3D11RenderTargetView* Dx11RenderTarget::get() { return render_target.Get(); }
