#include "StaticMeshRenderer.h"

#include <vulkan/vulkan_core.h>

#include "BufferBuilder.h"
#include "GraphicsPipelineBuilder.h"
#include "Matrix.h"
#include "MeshHandle.h"
#include "StaticModelData.h"
#include "TextureHandle.h"

namespace Graphics {

StaticMeshRenderer::StaticMeshRenderer(const EngineData& engine_data)
    : engine_data(engine_data),
      model_data_buffer(this->engine_data),
      next_object_index(0) {
    pipeline =
        GraphicsPipelineBuilder(
            "./Assets/Shaders/StaticModel/StaticModel.spv", "vertex_main",
            "./Assets/Shaders/StaticModel/StaticModel.spv", "pixel_main")
            .create(engine_data)
            .getResult();
}

void StaticMeshRenderer::queueMeshForRender(const FrameData& frame_data,
                                            const StaticModelData& model_data) {
    models.at(next_object_index) = model_data;
    next_object_index++;
}

void StaticMeshRenderer::render(const FrameData& frame_data) {
    TracyVkZone(frame_data.trace_ctx, frame_data.cmd.buffer, "Static mesh");

    auto command_buffer = frame_data.cmd;

    auto buffer_ptr = model_data_buffer.getHostAddress(frame_data);

    command_buffer.setPipeline(pipeline);

    for (int i = 0; i < next_object_index; i++) {
        StaticModelData model_data = models.at(i);

        StaticModelBuffer buffer = {};
        buffer.model = Matrix::translation(model_data.position);
        buffer.albedo_descriptor = model_data.albedo;

        buffer_ptr->at(i) = buffer;

        push_constants.model_data =
            model_data_buffer.getDeviceAddress(frame_data) +
            sizeof(StaticModelBuffer) * i;

        command_buffer.pushConstants(pipeline, &push_constants);
        auto mesh = engine_data.mesh_registry.getMesh(model_data.mesh);
        if (mesh) {
            command_buffer.draw(*mesh);
        }
    }

    next_object_index = 0;
}

void StaticMeshRenderer::setCameraData(const VkDeviceAddress camera_data) {
    push_constants.camera_data = camera_data;
}

}  // namespace Graphics