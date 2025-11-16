#pragma once

#include <type_traits>

#include "RenderTarget.h"
#include "Resource.h"
#include "Texture.h"

enum class ResourceType { Texture };

class RenderTargetBuilder {
   public:
    template <Resource T>
    RenderTargetBuilder(T resource)
        : resource(resource.get()), format(resource.getFormat()) {
        if constexpr (std::is_same_v<T, Texture>)
            type = ResourceType::Texture;
        else
            throw;
    }

    RenderTarget create();

   private:
    Microsoft::WRL::ComPtr<ID3D11Resource> resource;
    DXGI_FORMAT format;
    ResourceType type;
};