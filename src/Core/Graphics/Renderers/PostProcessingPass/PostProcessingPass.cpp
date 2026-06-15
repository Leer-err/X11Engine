#include "PostProcessingPass.h"

#include "AppConfig.h"
#include "Device.h"
#include "EngineData.h"
#include "FrameGraph.h"
#include "GraphicsPipelineBuilder.h"
#include "MeshBuilder.h"
#include "Overlay.h"
#include "RenderWorld.h"
#include "Sampler.h"
#include "Vector3.h"

namespace Graphics {

PostProcessingPass::PostProcessingPass(Device& device,
                                       const EngineData& engine_data)
    : engine_data(engine_data), dithering_data_buffer(device) {
    constexpr Vertex screen_quad_vertices[] = {
        Vertex(Vector3(-1, -1, 0), Vector2(0, 0)),
        Vertex(Vector3(-1, 1, 0), Vector2(0, 1)),
        Vertex(Vector3(1, -1, 0), Vector2(1, 0)),
        Vertex(Vector3(1, 1, 0), Vector2(1, 1))};

    constexpr uint32_t screen_quad_indices[] = {0, 1, 2, 1, 3, 2};

    quad = MeshBuilder(screen_quad_vertices, sizeof(screen_quad_vertices),
                       screen_quad_indices, sizeof(screen_quad_indices))
               .create(device, engine_data.staging_buffer);

    pipeline =
        GraphicsPipelineBuilder(
            "./Assets/Shaders/PostProcess/PostProcessing.spv", "mesh_main",
            "./Assets/Shaders/PostProcess/PostProcessing.spv", "pixel_main")
            .create(device, engine_data.shader_registry)
            .getResult();

    auto config = Config::App::get().getGraphicsConfig();

    data.spread = 0.1f;
    data.camera_dimensions = {config.render_width, config.render_height};
    data.color_count = 16;
    data.sampler_index = 0;

    data.sampler_index =
        engine_data.descriptor_set.addSampler(Sampler::point(device));

    Overlay::get().add<OverlayElements::SliderFloat>(
        "Graphics/Post processing", "Dithering spread",
        [this](float value) { data.spread = value; }, 0, 1, data.spread);
    Overlay::get().add<OverlayElements::SliderFloat>(
        "Graphics/Post processing", "Color count per channel",
        [this](float value) { data.color_count = value; }, 2, 256,
        data.color_count);
}

void PostProcessingPass::render(const Texture& input_image,
                                const FrameData& frame_data,
                                FrameGraph& frame_graph,
                                const RenderWorld& world) {
    // TracyVkZone(frame_data.trace_ctx, frame_data.cmd.buffer, "Dithering");

    auto render_target_opt = engine_data.descriptor_set.getIndex(input_image);
    if (render_target_opt.has_value() == false) return;

    data.render_target_index = *render_target_opt;
    dithering_data_buffer.update(frame_data, data);

    auto pass = GraphicsPass(pipeline, [this](GraphicsPassExecution& execution,
                                              const FrameData& frame_data) {
        auto data_address = dithering_data_buffer.getDeviceAddress(frame_data);
        execution.appendData(data_address);
        execution.draw(quad);
    });
    auto render_target =
        engine_data.texture_registry.getTexture("RenderResult");
    pass.addColorAttachment(*render_target, false, {});
    pass.reads(input_image);

    frame_graph.addGraphicsPass(pass);
}

}  // namespace Graphics