#include "DescriptorSet.h"

#include <vulkan/vulkan.h>

#include <bit>
#include <cstddef>
#include <cstdint>
#include <optional>

#include "Buffer.h"
#include "BufferBuilder.h"
#include "DeviceProperties.h"

namespace Graphics {

DescriptorSet::DescriptorSet(Device& device,
                             const DeviceProperties& device_properties)
    : device(device),
      descriptors(createDescriptorBuffer(
          device, device.getDescriptorLayout().layout_size,
          device_properties.descriptor_buffer_properties.alignment)),
      texture_allocator(device.getDescriptorLayout().texture_descriptors_count),
      sampler_allocator(
          device.getDescriptorLayout().sampler_descriptors_count) {
    auto& descriptor_properties =
        device_properties.descriptor_buffer_properties;

    auto descriptor_layout = device.getDescriptorLayout();

    texture_descriptor_size = descriptor_properties.texture_size;
    sampler_descriptor_size = descriptor_properties.sampler_size;
    texture_descriptors_offset = descriptor_layout.texture_descriptors_offset;
    sampler_descriptors_offset = descriptor_layout.sampler_descriptors_offset;
}

std::optional<TextureDescriptor> DescriptorSet::addTexture(
    VkImageView texture) {
    auto index = texture_allocator.allocate();
    if (index.has_value() == false) return std::nullopt;

    auto descriptors_ptr = getTextureDescriptorsData();
    auto descriptor_ptr =
        descriptors_ptr + texture_descriptor_size * index.value();

    VkDescriptorImageInfo image_descriptor_info = {};
    image_descriptor_info.imageView = texture;
    image_descriptor_info.imageLayout =
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkDescriptorGetInfoEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT;
    info.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    info.data.pSampledImage = &image_descriptor_info;
    device.writeDescriptor(info, texture_descriptor_size, descriptor_ptr);

    return index;
}

std::optional<TextureDescriptor> DescriptorSet::addSampler(VkSampler sampler) {
    auto index = sampler_allocator.allocate();
    if (index.has_value() == false) return std::nullopt;

    auto descriptors_ptr = getSamplerDescriptorsData();
    auto descriptor_ptr =
        descriptors_ptr + sampler_descriptor_size * index.value();

    VkDescriptorGetInfoEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT;
    info.type = VK_DESCRIPTOR_TYPE_SAMPLER;
    info.data.pSampler = &sampler;
    device.writeDescriptor(info, sampler_descriptor_size, descriptor_ptr);

    return index;
}

void DescriptorSet::removeTexture(TextureDescriptor texture) {
    texture_allocator.free(texture);
}

void DescriptorSet::removeSampler(SamplerDescriptor sampler) {
    sampler_allocator.free(sampler);
}

uint8_t* DescriptorSet::getTextureDescriptorsData() const {
    auto descriptors_ptr = descriptors.getHostAddress();

    return descriptors_ptr + texture_descriptors_offset;
}

uint8_t* DescriptorSet::getSamplerDescriptorsData() const {
    auto descriptors_ptr = descriptors.getHostAddress();

    return descriptors_ptr + sampler_descriptors_offset;
}

VkDeviceAddress DescriptorSet::getDescriptors() const {
    return descriptors.getDeviceAddress();
}

Buffer DescriptorSet::createDescriptorBuffer(Device& device, size_t set_size,
                                             size_t alignment) {
    auto aligned_size = (set_size + alignment - 1) & ~(alignment - 1);

    return BufferBuilder(aligned_size)
        .isDescriptorBuffer()
        .isCPUWritable(true, true)
        .create(device)
        .getResult();
}

}  // namespace Graphics