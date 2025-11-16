#pragma once

#include <type_traits>

#include "Resource.h"
#include "ShaderResource.h"
#include "Texture.h"

class ShaderResourceBuilder {
    enum class ResourceType { Texture };

   public:
    template <Resource T>
    ShaderResourceBuilder(T resource)
        : resource(resource.get()), format(resource.getFormat()) {
        if constexpr (std::is_same_v<T, Texture>)
            type = ResourceType::Texture;
        else
            throw;
    }

    ShaderResource create();

   private:
    Microsoft::WRL::ComPtr<ID3D11Resource> resource;
    DXGI_FORMAT format;
    ResourceType type;
};