#include "StaticMeshRenderer.h"

#include <vulkan/vulkan.h>

#include "GraphicsPipelineBuilder.h"
#include "Matrix.h"
#include "RenderWorld.h"
#include "Sampler.h"

namespace Graphics {

StaticMeshRenderer::StaticMeshRenderer(Device& device,
                                       const EngineData& engine_data)
    : engine_data(engine_data), model_data_buffer(device) {
    pipeline =
        GraphicsPipelineBuilder(
            "./Assets/Shaders/StaticModel/StaticModel.spv", "vertex_main",
            "./Assets/Shaders/StaticModel/StaticModel.spv", "pixel_main")
            .create(device, engine_data.shader_registry)
            .getResult();

    sampler_index =
        engine_data.descriptor_set.addSampler(Sampler::linear(device));
}

void StaticMeshRenderer::render(const FrameData& frame_data,
                                const RenderWorld& world) {
    // TracyVkZone(frame_data.trace_ctx, frame_data.cmd.buffer, "Static mesh");

    auto command_buffer = frame_data.cmd;

    auto buffer_ptr = model_data_buffer.getHostAddress(frame_data);

    command_buffer.setPipeline(pipeline);

    auto objects = world.getOpaqueObjects();
    for (int i = 0; i < objects.size(); i++) {
        const auto& model = objects[i];

        auto& model_data = (*buffer_ptr)[i];
        model_data.model = Matrix::translation(model.position);
        model_data.albedo_descriptor = model.albedo;
        model_data.albedo_sampler = sampler_index;

        push_constants.model_data =
            model_data_buffer.getDeviceAddress(frame_data) +
            sizeof(StaticModelBuffer) * i;

        command_buffer.pushConstants(pipeline, &push_constants);
        auto mesh = engine_data.mesh_registry.getMesh(model.mesh);
        command_buffer.draw(*mesh);
    }
}

void StaticMeshRenderer::setCameraData(const VkDeviceAddress camera_data) {
    push_constants.camera_data = camera_data;
}

}  // namespace Graphics