#include "RenderTarget.h"

#include "APIResources.h"
#include "Renderer.h"

RenderTarget RenderTarget::getDefault() {
    return Renderer::get().getDefaultRenderTarget();
}

RenderTarget::RenderTarget(const Texture& texture) {
    D3D11_RENDER_TARGET_VIEW_DESC desc = {};
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.Texture2D.MipSlice = 0;
    desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    auto device = APIResources::get().getDevice();

    device->CreateRenderTargetView(texture.get().Get(), &desc,
                                   render_target.GetAddressOf());

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Height = texture.getHeight();
    viewport.Width = texture.getWidth();
    viewport.MinDepth = 0.f;
    viewport.MaxDepth = 1.f;
}

Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTarget::get() const {
    return render_target;
}

D3D11_VIEWPORT RenderTarget::getViewport() const { return viewport; }
