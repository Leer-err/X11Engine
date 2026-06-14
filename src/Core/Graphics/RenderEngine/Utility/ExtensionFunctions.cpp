#include "ExtensionFunctions.h"

#include <vulkan/vulkan_core.h>

namespace Graphics {

// VK_EXT_descriptor_buffer
PFN_vkGetDescriptorSetLayoutSizeEXT vkGetDescriptorSetLayoutSizeEXT;
PFN_vkGetDescriptorSetLayoutBindingOffsetEXT
    vkGetDescriptorSetLayoutBindingOffsetEXT;
PFN_vkCmdBindDescriptorBuffersEXT vkCmdBindDescriptorBuffersEXT;
PFN_vkCmdSetDescriptorBufferOffsetsEXT vkCmdSetDescriptorBufferOffsetsEXT;
PFN_vkGetDescriptorEXT vkGetDescriptorEXT;
PFN_vkCmdBindDescriptorBufferEmbeddedSamplersEXT
    vkCmdBindDescriptorBufferEmbeddedSamplersEXT;

// VK_EXT_calibrated_timestamps
PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT
    vkGetPhysicalDeviceCalibrateableTimeDomainsEXT;
PFN_vkGetCalibratedTimestampsEXT vkGetCalibratedTimestampsEXT;

// VK_EXT_mesh_shader
PFN_vkCmdDrawMeshTasksEXT vkCmdDrawMeshTasksEXT;

void loadExtensionFunctions(VkInstance instance, VkDevice device) {
    // VK_EXT_descriptor_buffer
    vkGetDescriptorSetLayoutSizeEXT =
        reinterpret_cast<PFN_vkGetDescriptorSetLayoutSizeEXT>(
            vkGetDeviceProcAddr(device, "vkGetDescriptorSetLayoutSizeEXT"));
    vkGetDescriptorSetLayoutBindingOffsetEXT =
        reinterpret_cast<PFN_vkGetDescriptorSetLayoutBindingOffsetEXT>(
            vkGetDeviceProcAddr(device,
                                "vkGetDescriptorSetLayoutBindingOffsetEXT"));
    vkCmdBindDescriptorBuffersEXT =
        reinterpret_cast<PFN_vkCmdBindDescriptorBuffersEXT>(
            vkGetDeviceProcAddr(device, "vkCmdBindDescriptorBuffersEXT"));
    vkGetDescriptorEXT = reinterpret_cast<PFN_vkGetDescriptorEXT>(
        vkGetDeviceProcAddr(device, "vkGetDescriptorEXT"));
    vkCmdBindDescriptorBufferEmbeddedSamplersEXT =
        reinterpret_cast<PFN_vkCmdBindDescriptorBufferEmbeddedSamplersEXT>(
            vkGetDeviceProcAddr(
                device, "vkCmdBindDescriptorBufferEmbeddedSamplersEXT"));
    vkCmdSetDescriptorBufferOffsetsEXT =
        reinterpret_cast<PFN_vkCmdSetDescriptorBufferOffsetsEXT>(
            vkGetDeviceProcAddr(device, "vkCmdSetDescriptorBufferOffsetsEXT"));

    // VK_EXT_calibrated_timestamps
    vkGetPhysicalDeviceCalibrateableTimeDomainsEXT =
        reinterpret_cast<PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT>(
            vkGetInstanceProcAddr(
                instance, "vkGetPhysicalDeviceCalibrateableTimeDomainsEXT"));
    vkGetCalibratedTimestampsEXT =
        reinterpret_cast<PFN_vkGetCalibratedTimestampsEXT>(
            vkGetDeviceProcAddr(device, "vkGetCalibratedTimestampsEXT"));

    vkCmdDrawMeshTasksEXT = reinterpret_cast<PFN_vkCmdDrawMeshTasksEXT>(
        vkGetDeviceProcAddr(device, "vkCmdDrawMeshTasksEXT"));
}

}  // namespace Graphics