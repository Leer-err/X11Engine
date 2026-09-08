#pragma once

#include <vulkan/vulkan.h>

#include <cstddef>
#include <optional>
#include <vector>

#include "Buffer.h"
#include "Descriptors.h"
#include "Device.h"

namespace Graphics {

template <typename T>
class IndexAllocator {
   public:
    explicit IndexAllocator(T index_count) : count(index_count) {
        free_indices.reserve(count);

        for (auto i = count; i > 0; i--) free_indices.push_back(i);
    }

    std::optional<T> allocate() {
        if (free_indices.empty()) return std::nullopt;

        auto index = free_indices.back();
        free_indices.pop_back();

        return index;
    }

    void free(T index) { free_indices.push_back(index); }

   private:
    T count;
    std::vector<T> free_indices;
};

class DescriptorSet {
   public:
    DescriptorSet(Device& device, const DeviceProperties& device_properties);

    std::optional<TextureDescriptor> addTexture(VkImageView texture_view);
    std::optional<SamplerDescriptor> addSampler(VkSampler sampler);

    void removeTexture(TextureDescriptor texture);
    void removeSampler(SamplerDescriptor sampler);

    VkDeviceAddress getDescriptors() const;

   private:
    static Buffer createDescriptorBuffer(Device& device, size_t set_size,
                                         size_t alignment);

    uint8_t* getTextureDescriptorsData() const;
    uint8_t* getSamplerDescriptorsData() const;

    Device& device;

    Buffer descriptors;

    IndexAllocator<TextureDescriptor> texture_allocator;
    IndexAllocator<SamplerDescriptor> sampler_allocator;

    size_t texture_descriptor_size;
    size_t sampler_descriptor_size;
    size_t texture_descriptors_offset;
    size_t sampler_descriptors_offset;
};

}  // namespace Graphics