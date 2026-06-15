#include "TextureRegistry.h"

#include <string_view>

#include "Device.h"
#include "Texture.h"

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

Texture TextureRegistry::addTexture(const std::string& name,
                                    const TextureState& image) {
    auto result = addTexture(image);
    texture_names.try_emplace(name, result.getHandle());

    return result;
}

std::optional<Texture> TextureRegistry::getTexture(
    const TextureHandle& handle) {
    if (handle >= next_handle) return {};

    return Texture(handle, this);
}

std::optional<Texture> TextureRegistry::getTexture(std::string_view name) {
    auto it = texture_names.find(name);
    if (it == texture_names.end()) return {};

    return getTexture(it->second);
}

TextureState* TextureRegistry::getTextureState(const TextureHandle& handle) {
    if (handle >= next_handle) return nullptr;

    auto& state = states[handle];

    return &state;
}

}  // namespace Graphics