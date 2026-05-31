#pragma once

#include <optional>
#include <vector>

#include "Texture.h"
#include "TextureState.h"

namespace Graphics {

class TextureRegistry {
   public:
    TextureRegistry();

    Texture addTexture(const TextureState& image);
    std::optional<Texture> getTexture(const TextureHandle& handle);

   private:
    TextureHandle next_handle;
    std::vector<TextureState> states;
};

}  // namespace Graphics