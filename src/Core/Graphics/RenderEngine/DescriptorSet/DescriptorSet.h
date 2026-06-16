#pragma once

#include <vulkan/vulkan.h>

#include <cstddef>
#include <map>
#include <optional>

#include "Buffer.h"
#include "Device.h"
#include "Texture.h"

namespace Graphics {

class DescriptorSet {
   public:
    DescriptorSet(Device& device, BufferRegistry& registry,
                  const DeviceProperties& device_properties);

    uint32_t addTexture(const Texture& texture);
    uint32_t addSampler(const VkSampler& sampler);

    std::optional<uint32_t> getIndex(const Texture& texture);
    std::optional<uint32_t> getIndex(TextureHandle texture);

    VkDeviceAddress getDescriptors() const;

   private:
    static Buffer createDescriptorBuffer(Device& device,
                                         BufferRegistry& registry,
                                         size_t set_size, size_t alignment);

    Device& device;

    std::map<TextureHandle, uint32_t> texture_index_map;
    Buffer descriptors;
    TextureHandle current_texture_index;
    size_t current_sampler_index;

    size_t texture_descriptor_size;
    size_t sampler_descriptor_size;

    size_t texture_descriptors_offset;
    size_t sampler_descriptors_offset;
};

}  // namespace Graphics