#include "DitheringPass.h"

#include "EngineData.h"
#include "GraphicsPipelineBuilder.h"
#include "MeshBuilder.h"
#include "Vector3.h"

namespace Graphics {

DitheringPass::DitheringPass(Device& device, const EngineData& engine_data)
    : engine_data(engine_data), dithering_data_buffer(device) {
    constexpr Vector3 screen_quad_vertices[] = {
        Vector3(-1, -1, 0), Vector3(-1, 1, 0), Vector3(1, -1, 0),
        Vector3(1, 1, 0)};

    constexpr uint32_t screen_quad_indices[] = {0, 1, 2, 1, 3, 2};

    quad = MeshBuilder(screen_quad_vertices, sizeof(screen_quad_vertices),
                       screen_quad_indices, sizeof(screen_quad_indices))
               .create(device, engine_data.staging_buffer);

    pipeline = GraphicsPipelineBuilder(
                   "./Assets/Shaders/PostProcess/Dithering.spv", "vertex_main",
                   "./Assets/Shaders/PostProcess/Dithering.spv", "pixel_main")
                   .create(device, engine_data.shader_registry)
                   .getResult();
}

void DitheringPass::render(const FrameData& frame_data) {
    // TracyVkZone(frame_data.trace_ctx, frame_data.cmd.buffer, "Dithering");

    DitheringData data = {};
    data.spread = 0;
    data.camera_dimensions = {1280, 720};
    data.render_target_index = 0;
    data.sampler_index = 0;
    dithering_data_buffer.update(frame_data, data);

    auto command_buffer = frame_data.cmd;

    VkDeviceAddress data_address =
        dithering_data_buffer.getDeviceAddress(frame_data);

    command_buffer.setPipeline(pipeline);
    command_buffer.bindDescriptorSet(pipeline, engine_data.descriptor_set);
    command_buffer.pushConstants(pipeline, &data_address);

    command_buffer.draw(quad);
}

}  // namespace Graphics