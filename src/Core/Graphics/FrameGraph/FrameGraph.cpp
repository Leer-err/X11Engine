#include "FrameGraph.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <vector>

#include "CommandBuffer.h"
#include "DescriptorSet.h"
#include "EngineData.h"
#include "GraphicsPipeline.h"
#include "RenderEnviroment.h"

namespace Graphics {

VkImageMemoryBarrier2 Pass::fullBarrier(Texture& texture,
                                        VkImageLayout layout) {
    return texture.createBarrier(
        layout, VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
        VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT,
        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
        VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT);
}

VkImageMemoryBarrier2 Pass::depthStencilBarrier(Texture& texture) {
    return texture.createBarrier(
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
        VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT,
        VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
        VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
            VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
}

VkImageMemoryBarrier2 Pass::colorAttachmentBarrier(Texture& texture) {
    return texture.createBarrier(
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
        VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT |
            VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);
}

VkImageMemoryBarrier2 Pass::shaderResourceBarrier(Texture& texture) {
    return texture.createBarrier(
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
        VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT,
        VK_PIPELINE_STAGE_TASK_SHADER_BIT_EXT |
            VK_PIPELINE_STAGE_MESH_SHADER_BIT_EXT |
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        VK_ACCESS_2_SHADER_READ_BIT);
}

VkImageMemoryBarrier2 Pass::copySourceBarrier(Texture& texture) {
    return texture.createBarrier(
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
        VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT,
        VK_PIPELINE_STAGE_2_COPY_BIT, VK_ACCESS_2_TRANSFER_READ_BIT);
}

VkImageMemoryBarrier2 Pass::copyDestinatonBarrier(Texture& texture) {
    return texture.createBarrier(
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
        VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT,
        VK_PIPELINE_STAGE_2_COPY_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT);
}

VkImageMemoryBarrier2 Pass::barrierFromLayout(PassTexture& texture) {
    switch (texture.layout) {
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            return depthStencilBarrier(texture.texture);
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            return colorAttachmentBarrier(texture.texture);
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            return shaderResourceBarrier(texture.texture);
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            return copySourceBarrier(texture.texture);
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            return copyDestinatonBarrier(texture.texture);
        default:
            return fullBarrier(texture.texture, texture.layout);
    }
}

void Pass::prepareTextures(const CommandBuffer& command_buffer) {
    auto barrier_count_bound = pass_textures.size();

    auto barriers = std::vector<VkImageMemoryBarrier2>();
    barriers.reserve(barrier_count_bound);

    for (auto& texture : pass_textures) {
        auto barrier = barrierFromLayout(texture);
        barriers.emplace_back(barrier);
    }

    command_buffer.barrier(barriers);
}

void Pass::reads(const Texture& texture, VkImageLayout layout) {
    pass_textures.emplace_back(layout, texture);
}

void Pass::writes(const Texture& texture, VkImageLayout layout) {
    pass_textures.emplace_back(layout, texture);
}

GraphicsPass::GraphicsPass(
    const GraphicsPipeline& pipeline,
    const std::function<void(GraphicsPassExecution&)>& executor)
    : pipeline(pipeline), executor(executor) {}

void GraphicsPass::addColorAttachment(const Texture& texture) {
    color_attachments.emplace_back(texture, VkClearValue{}, false);
    writes(texture, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}

void GraphicsPass::addColorAttachment(const Texture& texture,
                                      VkClearValue clear_color) {
    color_attachments.emplace_back(texture, clear_color, true);
    writes(texture, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}

void GraphicsPass::setDepthAttachment(const Texture& texture) {
    depth_attachment = Attachment{texture, VkClearValue{}, false};
    writes(texture, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void GraphicsPass::setDepthAttachment(const Texture& texture,
                                      VkClearValue clear_value) {
    depth_attachment = Attachment{texture, clear_value, true};
    writes(texture, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void GraphicsPass::execute(const DescriptorSet& descriptor_set,
                           const CommandBuffer& command_buffer) {
    prepareTextures(command_buffer);

    RenderEnviroment env = {};
    if (!color_attachments.empty()) {
        const auto& attachment = color_attachments[0];
        const auto& texture_state = attachment.texture.getState();

        env.render_target = texture_state.view;
        env.clear_render_target = attachment.clear;
        env.render_target_clear_value = attachment.clear_value;
        env.width = texture_state.width;
        env.height = texture_state.height;
    }
    if (depth_attachment) {
        const auto& attachment = *depth_attachment;
        const auto& texture_state = attachment.texture.getState();

        env.depth_stencil = texture_state.view;
        env.clear_depth_stencil = attachment.clear;
        env.depth_stencil_clear_value = attachment.clear_value;
        env.width = texture_state.width;
        env.height = texture_state.height;
    }

    command_buffer.bindRenderEnviroment(env);

    GraphicsPassExecution execution(descriptor_set, command_buffer, pipeline);

    executor(execution);
}

struct MeshBuffers {
    VkDeviceAddress vertices;
    VkDeviceAddress meshlet_triangles;
    VkDeviceAddress meshlet_vertices;
    VkDeviceAddress meshlets;
};

GraphicsPassExecution::GraphicsPassExecution(
    const DescriptorSet& descriptor_set, const CommandBuffer& command_buffer,
    const GraphicsPipeline& pipeline)
    : command_buffer(command_buffer),
      pipeline(pipeline),
      current_push_constant_offset(sizeof(MeshBuffers)) {
    command_buffer.setPipeline(pipeline);
    command_buffer.bindDescriptorSet(pipeline, descriptor_set);
}

GraphicsPassExecution::~GraphicsPassExecution() {
    command_buffer.unbindRenderEnviroment();
}

void GraphicsPassExecution::appendData(const uint8_t* data, size_t data_size) {
    command_buffer.pushConstants(pipeline, data, current_push_constant_offset,
                                 data_size);
    current_push_constant_offset += data_size;
}

void GraphicsPassExecution::draw(const Mesh& mesh) {
    MeshBuffers push_data = {};

    push_data.vertices = mesh.vertex_buffer.getDeviceAddress();
    push_data.meshlet_triangles =
        mesh.meshlet_triangles_buffer.getDeviceAddress();
    push_data.meshlet_vertices =
        mesh.meshlet_vertices_buffer.getDeviceAddress();
    push_data.meshlets = mesh.meshlet_buffer.getDeviceAddress();

    command_buffer.pushConstants(pipeline, &push_data, 0);

    command_buffer.draw(mesh.meshlet_count);

    current_push_constant_offset = sizeof(MeshBuffers);
}

FrameGraph::FrameGraph(const EngineData& engine_data)
    : engine_data(engine_data) {}

void FrameGraph::addGraphicsPass(const GraphicsPass& pass) {
    passes.push_back(pass);
}

void FrameGraph::execute(const FrameData& frame_data) {
    for (auto& pass : passes) {
        pass.execute(engine_data.descriptor_set, frame_data.cmd);
    }
}

}  // namespace Graphics