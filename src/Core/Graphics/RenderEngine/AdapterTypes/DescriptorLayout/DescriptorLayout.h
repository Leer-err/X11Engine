#pragma once

#include <cstddef>

#include "vulkan/vulkan.h"

struct DescriptorLayout {
    VkDescriptorSetLayout layout;

    size_t layout_size;

    size_t texture_descriptors_offset;
    size_t texture_descriptors_count;
    size_t sampler_descriptors_offset;
    size_t sampler_descriptors_count;
};