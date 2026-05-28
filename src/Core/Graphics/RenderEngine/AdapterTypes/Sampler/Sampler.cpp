#include "Sampler.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "Device.h"
#include "EngineData.h"

namespace Graphics {

VkSampler Sampler::point(Device& device) {
    return createSampler(device, VK_FILTER_NEAREST, false, 0);
}

VkSampler Sampler::linear(Device& device) {
    return createSampler(device, VK_FILTER_LINEAR, false, 0);
}

VkSampler Sampler::anisotropic(Device& device, float anisotropy) {
    return createSampler(device, VK_FILTER_LINEAR, true, anisotropy);
}

VkSampler Sampler::createSampler(Device& device, VkFilter filter,
                                 bool anisotropic, float anisotropy) {
    VkSamplerMipmapMode mipmap_mode;
    switch (filter) {
        case VK_FILTER_NEAREST:
            mipmap_mode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
            break;
        case VK_FILTER_LINEAR:
            mipmap_mode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            break;
        default:
            return VK_NULL_HANDLE;
    }

    VkSamplerCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    info.magFilter = filter;
    info.magFilter = filter;
    info.mipmapMode = mipmap_mode;
    info.anisotropyEnable = anisotropic;
    info.maxAnisotropy = anisotropy;
    info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

    return device.createSampler(info);
}

}  // namespace Graphics