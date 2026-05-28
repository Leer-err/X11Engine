#pragma once

#include <VkBootstrap.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <cstddef>
#include <cstdint>
#include <tracy/TracyVulkan.hpp>

#include "Buffer.h"
#include "DescriptorLayout.h"
#include "DeviceProperties.h"
#include "GraphicsPipeline.h"
#include "Image.h"
#include "Logger.h"
#include "Queue.h"
#include "Result.h"
#include "Semaphore.h"

namespace Graphics {

struct CommandBuffer;

class Device {
   public:
    Device(const vkb::Instance& instance, const vkb::Device& device,
           VmaAllocator allocator);
    ~Device();

    vkb::Swapchain createSwapChain(VkSurfaceFormatKHR format,
                                   VkPresentModeKHR present_mode,
                                   size_t image_count, VkImageUsageFlags flags);

    Result<Image, ImageError> createImage(
        const VkImageCreateInfo& image_info,
        const VmaAllocationCreateInfo& alloc_info);

    Result<Buffer, BufferError> createBuffer(
        const VkBufferCreateInfo& buffer_info,
        const VmaAllocationCreateInfo& alloc_info);

    void* map(const Buffer& buffer) const;
    void unmap(const Buffer& buffer) const;

    VkCommandPool createCommandPool(uint32_t queue_index);
    void resetCommandPool(VkCommandPool pool) const;
    VkCommandBuffer createCommandBuffer(VkCommandPool pool);

    VkImageView createDepthStencil(const Image& image);
    VkImageView createTextureView(const Image& image);

    DescriptorLayout getDescriptorLayout() const;
    DeviceProperties getDeviceProperties() const;

    VkFence createFence(bool is_signaled);
    void waitFence(VkFence fence);
    void resetFence(VkFence fence) const;

    VkShaderModule createShader(const uint32_t* shader_data,
                                size_t shader_data_size);

    VkPipelineLayout createPipelineLayout(size_t push_constants_size);
    GraphicsPipeline createGraphicsPipeline(
        const VkGraphicsPipelineCreateInfo& pipeline_info);

    VkSampler createSampler(const VkSamplerCreateInfo& sampler_info);

    Semaphore createSemaphore();

    void writeDescriptor(const VkDescriptorGetInfoEXT& info,
                         size_t descriptor_size, void* dst) const;

    void waitIdle() const;

    VkInstance getInstance() const;
    VkDevice getDevice() const;
    VkPhysicalDevice getPhysicalDevice() const;

    TracyVkCtx createTracingContext(const Queue& queue,
                                    const CommandBuffer& command_buffer) const;

   private:
    void createDescriptorLayout();

    vkb::Instance instance;
    vkb::Device device;

    VmaAllocator allocator;

    DescriptorLayout descriptor_layout;
    DeviceProperties properties;

    Logger logger;
};

}  // namespace Graphics