#pragma once

#include <deque>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

#include "Device.h"
#include "Texture.h"
#include "TextureHandle.h"
#include "TextureState.h"

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

    struct TransparentStringHash {
        using is_transparent = void;

        size_t operator()(const char* str) const {
            return std::hash<std::string_view>{}(str);
        }
        size_t operator()(std::string_view str) const {
            return std::hash<std::string_view>{}(str);
        }
        size_t operator()(const std::string& str) const {
            return std::hash<std::string>{}(str);
        }
    };
    std::unordered_map<std::string, TextureHandle, TransparentStringHash,
                       std::equal_to<>>
        texture_names;
};

}  // namespace Graphics