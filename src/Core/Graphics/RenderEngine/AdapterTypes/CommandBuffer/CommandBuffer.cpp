#include "CommandBuffer.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <cstddef>

#include "Buffer.h"
#include "ExtensionFunctions.h"
#include "GraphicsPipeline.h"
#include "RenderEnviroment.h"
#include "Texture.h"

namespace Graphics {

void CommandBuffer::begin() const {
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(buffer, &info);
}

void CommandBuffer::copy(const TextureState& src, TextureState& dst) const {
    VkImageCopy2 copy_region = {};
    copy_region.sType = VK_STRUCTURE_TYPE_IMAGE_COPY_2;
    copy_region.extent.width = src.width;
    copy_region.extent.height = src.height;
    copy_region.extent.depth = 1;
    copy_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copy_region.srcSubresource.layerCount = 1;
    copy_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copy_region.dstSubresource.layerCount = 1;

    VkCopyImageInfo2 copy_info = {};
    copy_info.sType = VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2;
    copy_info.srcImage = src.texture;
    copy_info.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    copy_info.dstImage = dst.texture;
    copy_info.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    copy_info.regionCount = 1;
    copy_info.pRegions = &copy_region;

    vkCmdCopyImage2(buffer, &copy_info);
}

void CommandBuffer::blit(const TextureState& src, TextureState& dst) const {
    VkImageBlit2 region = {};
    region.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
    region.srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    region.srcOffsets[0] = {};
    region.srcOffsets[1] = {static_cast<int32_t>(src.width),
                            static_cast<int32_t>(src.height), 1};
    region.dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    region.dstOffsets[0] = {};
    region.dstOffsets[1] = {static_cast<int32_t>(dst.width),
                            static_cast<int32_t>(dst.height), 1};

    VkBlitImageInfo2 info = {};
    info.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
    info.srcImage = src.texture;
    info.srcImageLayout = src.layout;
    info.dstImage = dst.texture;
    info.dstImageLayout = dst.layout;
    info.filter = VK_FILTER_NEAREST;
    info.regionCount = 1;
    info.pRegions = &region;

    vkCmdBlitImage2(buffer, &info);
}

void CommandBuffer::pushConstants(const GraphicsPipeline& pipeline,
                                  const void* constants,
                                  size_t constant_size) const {
    vkCmdPushConstants(buffer, pipeline.layout, VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                       constant_size, constants);
}

void CommandBuffer::setPipeline(const GraphicsPipeline& pipeline) const {
    vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      pipeline.pipeline);
}

void CommandBuffer::draw(const Mesh& mesh) const {
    vkCmdBindIndexBuffer(buffer, mesh.index_buffer.buffer, 0,
                         VK_INDEX_TYPE_UINT32);

    VkDeviceSize vertex_buffer_offset = 0;
    vkCmdBindVertexBuffers(buffer, 0, 1, &mesh.vertex_buffer.buffer,
                           &vertex_buffer_offset);

    auto index_count = mesh.index_buffer.size / sizeof(uint32_t);
    vkCmdDrawIndexed(buffer, index_count, 1, 0, 0, 0);
}

void CommandBuffer::bindDescriptorSet(const GraphicsPipeline& pipeline,
                                      const DescriptorSet& set) const {
    VkDescriptorBufferBindingInfoEXT info = {};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_INFO_EXT;
    info.address = set.getDescriptors();
    info.usage = VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT;
    vkCmdBindDescriptorBuffersEXT(buffer, 1, &info);

    uint32_t indices = {};
    VkDeviceSize offsets = {};
    vkCmdSetDescriptorBufferOffsetsEXT(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                       pipeline.layout, 0, 1, &indices,
                                       &offsets);
}

void CommandBuffer::bindRenderEnviroment(const RenderEnviroment& env) const {
    VkRenderingInfo render_info = {};
    render_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    render_info.renderArea.extent.width = env.width;
    render_info.renderArea.extent.height = env.height;
    render_info.layerCount = 1;

    VkRenderingAttachmentInfo colorAttachmentInfo = {};
    colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachmentInfo.imageView = env.render_target;
    colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
    colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    if (env.clear_render_target) {
        colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachmentInfo.clearValue.color =
            env.render_target_clear_value.color;
    }

    VkRenderingAttachmentInfo depthAttachmentInfo{
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = env.depth_stencil,
        .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE};

    if (env.clear_render_target) {
        depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachmentInfo.clearValue.depthStencil = {env.clear_depth,
                                                       env.clear_stencil};
    }

    render_info.colorAttachmentCount = 1;
    render_info.pColorAttachments = &colorAttachmentInfo;

    if (env.depth_stencil != VK_NULL_HANDLE)
        render_info.pDepthAttachment = &depthAttachmentInfo;

    vkCmdBeginRendering(buffer, &render_info);

    VkViewport viewport = {};
    viewport.width = static_cast<float>(env.width);
    viewport.height = static_cast<float>(env.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(buffer, 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.extent.width = static_cast<uint32_t>(env.width);
    scissor.extent.height = static_cast<uint32_t>(env.height);
    vkCmdSetScissor(buffer, 0, 1, &scissor);
}

void CommandBuffer::unbindRenderEnviroment() const {
    vkCmdEndRendering(buffer);
}

void CommandBuffer::end() const { vkEndCommandBuffer(buffer); }

void CommandBuffer::reset() const { vkResetCommandBuffer(buffer, 0); }

void CommandBuffer::barrier(const VkImageMemoryBarrier2* image_barriers,
                            size_t image_barrier_count,
                            const VkBufferMemoryBarrier2* buffer_barriers,
                            size_t buffer_barrier_count) const {
    VkDependencyInfo dep = {};
    dep.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dep.imageMemoryBarrierCount = image_barrier_count;
    dep.pImageMemoryBarriers = image_barriers;
    dep.pBufferMemoryBarriers = buffer_barriers;
    dep.bufferMemoryBarrierCount = buffer_barrier_count;

    vkCmdPipelineBarrier2(buffer, &dep);
}

VkCommandBufferSubmitInfo CommandBuffer::submit() const {
    VkCommandBufferSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    info.commandBuffer = buffer;
    info.deviceMask = 0;

    return info;
}

}  // namespace Graphics