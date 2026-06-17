#include "StarRenderer.h"

#include <vulkan/vulkan.h>

#include <array>
#include <cstring>

#include "BufferBuilder.h"
#include "CommandBuffer.h"
#include "Device.h"
#include "EngineData.h"
#include "GraphicsMesh.h"
#include "GraphicsPipelineBuilder.h"
#include "MeshBuilder.h"
#include "RenderWorld/RenderWorld.h"
#include "StarsData.h"
#include "Vector2.h"
#include "Vector3.h"

namespace Graphics {

StarRenderer::StarRenderer(Device& device, const EngineData& engine_data)
    : engine_data(engine_data),
      quad(createScreenQuad(device, engine_data)),
      stars_data_buffer(createStarsBuffer(device)) {
    pipeline = GraphicsPipelineBuilder(
                   "./Assets/Shaders/Stars/Stars.spv", "vertex_main",
                   "./Assets/Shaders/Stars/Stars.spv", "pixel_main")
                   .create(device, engine_data.shader_registry)
                   .getResult();
}

void StarRenderer::render(const FrameData& frame_data,
                          const RenderWorld& world) {
    // TracyVkZone(frame_data.trace_ctx, frame_data.cmd.buffer, "Stars");

    auto command_buffer = frame_data.cmd;

    auto stars_data = world.getStarsData();
    stars_data_buffer.update(stars_data);
    push_constants.stars_data = stars_data_buffer.getDeviceAddress();

    command_buffer.setPipeline(pipeline);

    command_buffer.pushConstants(pipeline, &push_constants, 0);

    // command_buffer.draw(quad);
}

void StarRenderer::setCameraData(VkDeviceAddress camera_data) {
    push_constants.camera_data = camera_data;
}

Buffer StarRenderer::createStarsBuffer(Device& device) {
    return BufferBuilder(sizeof(StarsData))
        .isConstantBuffer()
        .isChained()
        .isDeviceAddressable()
        .create(device)
        .getResult();
}

Mesh createScreenQuad(Device& device, const EngineData& engine_data) {
    constexpr std::array<Vertex, 4> screen_quad_vertices = {
        Vertex{Vector3(-1, -1, 1), Vector2{0, 0}},
        Vertex{Vector3(-1, 1, 1), Vector2{0, 0}},
        Vertex{Vector3(1, -1, 1), Vector2{0, 0}},
        Vertex{Vector3(1, 1, 1), Vector2{0, 0}}};

    constexpr std::array<uint32_t, 6> screen_quad_indices = {0, 1, 2, 1, 3, 2};

    return MeshBuilder(screen_quad_vertices.data(),
                       sizeof(screen_quad_vertices), screen_quad_indices.data(),
                       sizeof(screen_quad_indices))
        .create(device, engine_data.staging_buffer);
}

}  // namespace Graphics