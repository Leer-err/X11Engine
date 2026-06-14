#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace Graphics {

void loadExtensionFunctions(VkInstance instance, VkDevice device);

// VK_EXT_descriptor_buffer
extern PFN_vkGetDescriptorSetLayoutSizeEXT vkGetDescriptorSetLayoutSizeEXT;
extern PFN_vkGetDescriptorSetLayoutBindingOffsetEXT
    vkGetDescriptorSetLayoutBindingOffsetEXT;
extern PFN_vkCmdBindDescriptorBuffersEXT vkCmdBindDescriptorBuffersEXT;
extern PFN_vkCmdSetDescriptorBufferOffsetsEXT
    vkCmdSetDescriptorBufferOffsetsEXT;
extern PFN_vkGetDescriptorEXT vkGetDescriptorEXT;
extern PFN_vkCmdBindDescriptorBufferEmbeddedSamplersEXT
    vkCmdBindDescriptorBufferEmbeddedSamplersEXT;

// VK_EXT_calibrated_timestamps
extern PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT
    vkGetPhysicalDeviceCalibrateableTimeDomainsEXT;
extern PFN_vkGetCalibratedTimestampsEXT vkGetCalibratedTimestampsEXT;

// VK_EXT_mesh_shader
extern PFN_vkCmdDrawMeshTasksEXT vkCmdDrawMeshTasksEXT;

}  // namespace Graphics