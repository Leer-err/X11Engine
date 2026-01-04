#include "DepthStencilBuilder.h"

#include <d3d11.h>
#include <wrl/client.h>

#include "APIResources.h"
#include "DepthStencil.h"

DepthStencil DepthStencilBuilder::create() {
    D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
    desc.Format = format;
    desc.Texture2D.MipSlice = 0;

    switch (type) {
        case ResourceType::Texture:
            desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            break;
    }

    auto device = APIResources::get().getDevice();

    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> render_target;
    device->CreateDepthStencilView(resource.Get(), &desc,
                                   render_target.GetAddressOf());
    return DepthStencil(render_target);
}