#include "PostProcessingPass.h"

#include <vulkan/vulkan_core.h>

#include <array>

#include "AppConfig.h"
#include "BufferBuilder.h"
#include "Device.h"
#include "EngineData.h"
#include "FrameGraph.h"
#include "GraphicsPipelineBuilder.h"
#include "MeshBuilder.h"
#include "Overlay.h"
#include "PostProcessingData.h"
#include "RenderWorld.h"
#include "Sampler.h"
#include "Vector3.h"

namespace Graphics {

PostProcessingPass::PostProcessingPass(Device& device,
                                       const EngineData& engine_data)
    : engine_data(engine_data),
      quad(createSqreenQuad(device, engine_data)),
      dithering_data_buffer(createDataBuffer(device)) {
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
                                FrameGraph& frame_graph,
                                const RenderWorld& world) {
    // TracyVkZone(frame_data.trace_ctx, frame_data.cmd.buffer, "Dithering");

    auto render_target_opt = engine_data.descriptor_set.getIndex(input_image);
    if (render_target_opt.has_value() == false) return;

    data.render_target_index = *render_target_opt;
    dithering_data_buffer.update(data);

    auto pass =
        GraphicsPass(pipeline, [this](GraphicsPassExecution& execution) {
            auto data_address = dithering_data_buffer.getDeviceAddress();
            execution.appendData(data_address);
            execution.draw(quad);
        });
    auto render_target =
        engine_data.texture_registry.getTexture("RenderResult");
    pass.addColorAttachment(*render_target);
    pass.reads(input_image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    frame_graph.addGraphicsPass(pass);
}

Mesh PostProcessingPass::createSqreenQuad(Device& device,
                                          const EngineData& engine_data) {
    constexpr std::array<Vertex, 4> screen_quad_vertices = {
        Vertex{Vector3{-1, -1, 0}, Vector2{0, 0}},
        Vertex{Vector3{-1, 1, 0}, Vector2{0, 1}},
        Vertex{Vector3{1, -1, 0}, Vector2{1, 0}},
        Vertex{Vector3{1, 1, 0}, Vector2{1, 1}}};

    constexpr std::array<uint32_t, 6> screen_quad_indices = {0, 1, 2, 1, 3, 2};

    return MeshBuilder(screen_quad_vertices.data(),
                       sizeof(screen_quad_vertices), screen_quad_indices.data(),
                       sizeof(screen_quad_indices))
        .create(device, engine_data.staging_buffer);
}

Buffer PostProcessingPass::createDataBuffer(Device& device) {
    return BufferBuilder(sizeof(PostProcessingData))
        .isConstantBuffer()
        .isDeviceAddressable()
        .isChained()
        .isCPUWritable(true)
        .create(device)
        .getResult();
}

}  // namespace Graphics