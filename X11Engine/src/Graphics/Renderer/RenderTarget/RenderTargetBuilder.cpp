#include "RenderTargetBuilder.h"

#include <d3d11.h>
#include <wrl/client.h>

#include "APIResources.h"
#include "RenderTarget.h"

RenderTarget RenderTargetBuilder::create() {
    D3D11_RENDER_TARGET_VIEW_DESC desc = {};
    desc.Format = format;
    desc.Texture2D.MipSlice = 0;

    switch (type) {
        case ResourceType::Texture:
            desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            break;
    }

    auto device = APIResources::get().getDevice();

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target;
    device->CreateRenderTargetView(resource.Get(), &desc,
                                   render_target.GetAddressOf());
    return RenderTarget(render_target);
}