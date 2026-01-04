#pragma once

#include <type_traits>

#include "DepthStencil.h"
#include "Resource.h"
#include "Texture.h"

class DepthStencilBuilder {
    enum class ResourceType { Texture };

   public:
    template <Resource T>
    DepthStencilBuilder(T resource)
        : resource(resource.get()), format(resource.getFormat()) {
        if constexpr (std::is_same_v<T, Texture>)
            type = ResourceType::Texture;
        else
            throw;
    }

    DepthStencil create();

   private:
    Microsoft::WRL::ComPtr<ID3D11Resource> resource;
    DXGI_FORMAT format;
    ResourceType type;
};