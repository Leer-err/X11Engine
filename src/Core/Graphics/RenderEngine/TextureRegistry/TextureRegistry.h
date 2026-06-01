#pragma once

#include <deque>
#include <optional>

#include "Device.h"
#include "Texture.h"
#include "TextureState.h"

namespace Graphics {

class TextureRegistry {
   public:
    explicit TextureRegistry(Device& device);
    ~TextureRegistry();

    Texture addTexture(const TextureState& image);
    std::optional<Texture> getTexture(const TextureHandle& handle);
    TextureState* getTextureState(const TextureHandle& handle);

   private:
    Device& device;

    TextureHandle next_handle;
    std::deque<TextureState> states;
};

}  // namespace Graphics