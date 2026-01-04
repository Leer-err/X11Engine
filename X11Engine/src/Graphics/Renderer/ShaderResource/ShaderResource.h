#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <type_traits>

#include "Texture.h"

class ShaderResource {
    friend class Context;

    enum class ResourceType { Texture };

   public:
    ShaderResource() = default;
    template <typename T>
    ShaderResource(const T& resource) {
        if constexpr (std::is_same_v<T, Texture>)
            createView(resource.get(), resource.getFormat(),
                       ResourceType::Texture);
        else
            throw;
    }

   protected:
    void createView(Microsoft::WRL::ComPtr<ID3D11Resource> resource,
                    DXGI_FORMAT format, ResourceType type);

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> get() const;

   private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource;
};