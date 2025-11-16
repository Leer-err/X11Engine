#include "ShaderResourceBuilder.h"

#include <d3dcommon.h>

#include "APIResources.h"
#include "ShaderResource.h"

ShaderResource ShaderResourceBuilder::create() {
    D3D11_SHADER_RESOURCE_VIEW_DESC view_descriptor = {};
    view_descriptor.Format = format;

    if (type == ResourceType::Texture) {
        view_descriptor.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        view_descriptor.Texture2D.MipLevels = 1;
    }

    auto device = APIResources::get().getDevice();

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view;
    HRESULT result = device->CreateShaderResourceView(resource.Get(),
                                                      &view_descriptor, &view);

    return ShaderResource(view);
}
