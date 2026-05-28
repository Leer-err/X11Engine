#include "DitheringPass.h"

#include "EngineData.h"
#include "GraphicsPipelineBuilder.h"
#include "MeshBuilder.h"
#include "Overlay.h"
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

    data.spread = 0.1;
    data.camera_dimensions = {1280, 720};
    data.color_count = 16;
    data.sampler_index = 0;

    Overlay::get().add<OverlayElements::SliderFloat>(
        "Graphics/Post processing", "Dithering spread",
        [this](float value) { data.spread = value; }, 0, 1, data.spread);
    Overlay::get().add<OverlayElements::SliderFloat>(
        "Graphics/Post processing", "Color count per channel",
        [this](float value) { data.color_count = value; }, 2, 256,
        data.color_count);
}

void DitheringPass::render(TextureHandle input_image,
                           const FrameData& frame_data) {
    // TracyVkZone(frame_data.trace_ctx, frame_data.cmd.buffer, "Dithering");

    auto command_buffer = frame_data.cmd;

    data.render_target_index = input_image;
    dithering_data_buffer.update(frame_data, data);

    VkDeviceAddress data_address =
        dithering_data_buffer.getDeviceAddress(frame_data);

    command_buffer.setPipeline(pipeline);
    command_buffer.bindDescriptorSet(pipeline, engine_data.descriptor_set);
    command_buffer.pushConstants(pipeline, &data_address);

    command_buffer.draw(quad);
}

}  // namespace Graphics