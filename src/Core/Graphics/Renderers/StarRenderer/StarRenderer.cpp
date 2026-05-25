#include "StarRenderer.h"

#include <vulkan/vulkan.h>

#include <cstring>

#include "BufferBuilder.h"
#include "CommandBuffer.h"
#include "EngineData.h"
#include "GraphicsPipelineBuilder.h"
#include "MeshBuilder.h"
#include "StarsData.h"
#include "Vector3.h"

namespace Graphics {

StarRenderer::StarRenderer(const EngineData& engine_data)
    : engine_data(engine_data), stars_data_buffer(this->engine_data) {
    constexpr Vector3 screen_quad_vertices[] = {
        Vector3(-1, -1, 1), Vector3(-1, 1, 1), Vector3(1, -1, 1),
        Vector3(1, 1, 1)};

    constexpr uint32_t screen_quad_indices[] = {0, 1, 2, 1, 3, 2};

    quad = MeshBuilder(screen_quad_vertices, sizeof(screen_quad_vertices),
                       screen_quad_indices, sizeof(screen_quad_indices))
               .create(engine_data);

    pipeline = GraphicsPipelineBuilder(
                   "./Assets/Shaders/Stars/Stars.spv", "vertex_main",
                   "./Assets/Shaders/Stars/Stars.spv", "pixel_main")
                   .create(engine_data)
                   .getResult();
}

void StarRenderer::render(const FrameData& frame_data,
                          const StarsData& stars_data) {
    TracyVkZone(frame_data.trace_ctx, frame_data.cmd.buffer, "Stars");

    auto command_buffer = frame_data.cmd;

    stars_data_buffer.update(frame_data, stars_data);
    push_constants.stars_data = stars_data_buffer.getDeviceAddress(frame_data);

    command_buffer.setPipeline(pipeline);

    command_buffer.pushConstants(pipeline, &push_constants);

    command_buffer.draw(quad);
}

void StarRenderer::setCameraData(VkDeviceAddress camera_data) {
    push_constants.camera_data = camera_data;
}

}  // namespace Graphics