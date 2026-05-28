#pragma once

#include <optional>
#include <vector>

#include "Image.h"
#include "TextureHandle.h"

namespace Graphics {

class TextureRegistry {
   public:
    TextureRegistry();

    TextureHandle addTexture(const Image& image);
    std::optional<Image> getTexture(const TextureHandle& handle);

   private:
    TextureHandle next_handle;
    std::vector<Image> images;
};

}  // namespace Graphics