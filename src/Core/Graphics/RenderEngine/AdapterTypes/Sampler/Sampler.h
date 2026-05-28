#pragma once

#include <vulkan/vulkan.h>

#include "Device.h"

namespace Graphics {

class Sampler {
   public:
    static VkSampler point(Device& device);
    static VkSampler linear(Device& device);
    static VkSampler anisotropic(Device& device, float anisotropy);

   private:
    static VkSampler createSampler(Device& device, VkFilter filter,
                                   bool anisotropic, float anisotropy);
};

}  // namespace Graphics