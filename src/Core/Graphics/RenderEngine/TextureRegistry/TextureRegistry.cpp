#include "TextureRegistry.h"

namespace Graphics {

TextureRegistry::TextureRegistry() : next_handle(0) {}

TextureHandle TextureRegistry::addTexture(const Image& image) {
    images.push_back(image);

    return next_handle++;
}

std::optional<Image> TextureRegistry::getTexture(const TextureHandle& handle) {
    if (handle >= next_handle) return {};

    return images[handle];
}

}  // namespace Graphics