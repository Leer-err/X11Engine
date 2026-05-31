#include "TextureRegistry.h"

namespace Graphics {

TextureRegistry::TextureRegistry() : next_handle(0) {}

Texture TextureRegistry::addTexture(const TextureState& image) {
    auto& state = states.emplace_back(image);

    return Texture(next_handle++, &state, this);
}

std::optional<Texture> TextureRegistry::getTexture(
    const TextureHandle& handle) {
    if (handle >= next_handle) return {};

    auto& state = states[handle];

    return Texture(handle, &state, this);
}

}  // namespace Graphics