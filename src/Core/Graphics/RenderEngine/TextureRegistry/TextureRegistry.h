#pragma once

#include <deque>
#include <functional>
#include <optional>
#include <unordered_map>

#include "Device.h"
#include "Texture.h"
#include "TextureHandle.h"
#include "TextureState.h"
#include "TransparentStringHash.h"

namespace Graphics {

class TextureRegistry {
   public:
    explicit TextureRegistry(Device& device);
    ~TextureRegistry();

    Texture addTexture(const TextureState& image);
    Texture addTexture(const std::string& name, const TextureState& image);
    std::optional<Texture> getTexture(const TextureHandle& handle);
    std::optional<Texture> getTexture(std::string_view name);
    TextureState* getTextureState(const TextureHandle& handle);

   private:
    Device& device;

    TextureHandle next_handle;
    std::deque<TextureState> states;

    std::unordered_map<std::string, TextureHandle, TransparentStringHash,
                       std::equal_to<>>
        texture_names;
};

}  // namespace Graphics