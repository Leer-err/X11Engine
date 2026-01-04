#include "ShaderResource.h"

#include "APIResources.h"

void ShaderResource::createView(Microsoft::WRL::ComPtr<ID3D11Resource> resource,
                                DXGI_FORMAT format, ResourceType type) {
    D3D11_SHADER_RESOURCE_VIEW_DESC view_descriptor = {};
    view_descriptor.Format = format;

    if (type == ResourceType::Texture) {
        view_descriptor.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        view_descriptor.Texture2D.MipLevels = 1;
    }

    auto device = APIResources::get().getDevice();

    HRESULT result = device->CreateShaderResourceView(
        resource.Get(), &view_descriptor, &shader_resource);
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResource::get() const {
    return shader_resource;
}
