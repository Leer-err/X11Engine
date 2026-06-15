#include "FrameGraph.h"

#include <vulkan/vulkan_core.h>

#include <vector>

#include "CommandBuffer.h"
#include "DescriptorSet.h"
#include "EngineData.h"
#include "GraphicsPipeline.h"
#include "RenderEnviroment.h"

namespace Graphics {

GraphicsPass::GraphicsPass(
    const GraphicsPipeline& pipeline,
    const std::function<void(GraphicsPassExecution&, const FrameData&)>& pass)
    : pipeline(pipeline), pass_function(pass), writes_depth(false) {}

void GraphicsPass::reads(const Texture& texture) {
    read_textures.emplace_back(texture);
}

void GraphicsPass::addColorAttachment(const Texture& texture, bool clear,
                                      VkClearValue clear_color) {
    color_attachments.emplace_back(texture, clear, clear_color);
}

void GraphicsPass::addDepthAttachment(const Texture& texture, bool writes,
                                      bool clear, VkClearValue clear_value) {
    depth_attachment = Attachment{texture, clear, clear_value};
    writes_depth = writes;
}

void GraphicsPass::execute(const DescriptorSet& descriptor_set,
                           const FrameData& frame_data) {
    auto& command_buffer = frame_data.cmd;

    auto barrier_count_bound = read_textures.size() + color_attachments.size();
    if (depth_attachment) barrier_count_bound++;

    auto barriers = std::vector<VkImageMemoryBarrier2>();
    barriers.reserve(barrier_count_bound);

    for (auto& texture : read_textures) {
        auto barrier = texture.createBarrier(
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
            VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT,
            VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
            VK_ACCESS_2_SHADER_SAMPLED_READ_BIT);
        barriers.push_back(barrier);
    }

    for (auto& attachment : color_attachments) {
        auto barrier = attachment.texture.createBarrier(
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
            VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT,
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);
        barriers.push_back(barrier);
    }

    if (depth_attachment) {
        auto barrier = depth_attachment->texture.createBarrier(
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
            VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT,
            VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT_KHR,
            VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT);
        if (writes_depth)
            barrier.dstAccessMask |=
                VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        barriers.push_back(barrier);
    }

    command_buffer.barrier(barriers);

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

    pass_function(execution, frame_data);
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

    push_data.vertices = mesh.vertex_buffer.device_address;
    push_data.meshlet_triangles = mesh.meshlet_triangles_buffer.device_address;
    push_data.meshlet_vertices = mesh.meshlet_vertices_buffer.device_address;
    push_data.meshlets = mesh.meshlet_buffer.device_address;

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
        pass.execute(engine_data.descriptor_set, frame_data);
    }
}

}  // namespace Graphics