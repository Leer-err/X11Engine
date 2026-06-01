#include "TextureRegistry.h"

#include "Device.h"

namespace Graphics {

TextureRegistry::TextureRegistry(Device& device)
    : device(device), next_handle(0) {}

TextureRegistry::~TextureRegistry() {
    for (const auto& state : states) {
        device.destroyTexture(state);
    }
}

Texture TextureRegistry::addTexture(const TextureState& image) {
    states.emplace_back(image);

    return Texture(next_handle++, this);
}

std::optional<Texture> TextureRegistry::getTexture(
    const TextureHandle& handle) {
    if (handle >= next_handle) return {};

    return Texture(handle, this);
}

TextureState* TextureRegistry::getTextureState(const TextureHandle& handle) {
    if (handle >= next_handle) return nullptr;

    auto& state = states[handle];

    return &state;
}

}  // namespace Graphics