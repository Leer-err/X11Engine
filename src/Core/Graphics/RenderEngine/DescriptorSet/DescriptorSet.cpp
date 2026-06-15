#include "DescriptorSet.h"

#include <vulkan/vulkan_core.h>

#include <cstdint>

#include "Buffer.h"
#include "BufferBuilder.h"
#include "DeviceProperties.h"

namespace Graphics {

DescriptorSet::DescriptorSet(Device& device,
                             const DeviceProperties& device_properties)
    : device(device) {
    auto& descriptor_properties =
        device_properties.descriptor_buffer_properties;

    auto descriptor_layout = device.getDescriptorLayout();

    texture_descriptor_size = descriptor_properties.texture_size;
    sampler_descriptor_size = descriptor_properties.sampler_size;
    texture_descriptors_offset = descriptor_layout.texture_descriptors_offset;
    sampler_descriptors_offset = descriptor_layout.sampler_descriptors_offset;

    size_t set_size = descriptor_layout.layout_size;
    auto alignment = descriptor_properties.alignment;
    set_size = (set_size + alignment - 1) & ~(alignment - 1);

    current_sampler_index = 0;
    current_texture_index = 0;

    descriptors = BufferBuilder(set_size)
                      .isDescriptorBuffer()
                      .isCPUWritable(true, true)
                      .create(device)
                      .getResult();
}

uint32_t DescriptorSet::addTexture(const Texture& texture) {
    auto descriptors_ptr = descriptors.mapped_address;

    char* binding_ptr =
        static_cast<char*>(descriptors_ptr) + texture_descriptors_offset;
    char* element_ptr =
        binding_ptr + (current_texture_index * texture_descriptor_size);

    auto view = texture.getState().view;

    VkDescriptorImageInfo image_descriptor_info = {};
    image_descriptor_info.imageView = view;
    image_descriptor_info.imageLayout =
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkDescriptorGetInfoEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT;
    info.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    info.data.pSampledImage = &image_descriptor_info;
    device.writeDescriptor(info, texture_descriptor_size, element_ptr);

    auto index = current_texture_index;
    texture_index_map.emplace(texture.getHandle(), index);
    current_texture_index++;

    return index;
}

uint32_t DescriptorSet::addSampler(const VkSampler& sampler) {
    auto descriptors_ptr = descriptors.mapped_address;

    char* binding_ptr =
        static_cast<char*>(descriptors_ptr) + sampler_descriptors_offset;
    char* element_ptr =
        binding_ptr + (current_sampler_index * sampler_descriptor_size);

    VkDescriptorGetInfoEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT;
    info.type = VK_DESCRIPTOR_TYPE_SAMPLER;
    info.data.pSampler = &sampler;
    device.writeDescriptor(info, sampler_descriptor_size, element_ptr);

    auto index = current_sampler_index;
    current_sampler_index++;

    return index;
}

std::optional<uint32_t> DescriptorSet::getIndex(const Texture& texture) {
    return getIndex(texture.getHandle());
}

std::optional<uint32_t> DescriptorSet::getIndex(TextureHandle texture) {
    auto it = texture_index_map.find(texture);
    if (it == texture_index_map.end()) return {};

    return it->second;
}

VkDeviceAddress DescriptorSet::getDescriptors() const {
    return descriptors.device_address;
}

}  // namespace Graphics