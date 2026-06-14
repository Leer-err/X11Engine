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
    pipeline = GraphicsPipelineBuilder(
                   "./Assets/Shaders/StaticModel/StaticModel.spv", "mesh_main",
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
    command_buffer.bindDescriptorSet(pipeline, engine_data.descriptor_set);

    auto objects = world.getOpaqueObjects();
    for (int i = 0; i < objects.size(); i++) {
        const auto& model = objects[i];

        auto& model_data = (*buffer_ptr)[i];
        model_data.model = Matrix::translation(model.position);
        model_data.albedo_descriptor = model.albedo;
        model_data.albedo_sampler = sampler_index;

        auto mesh = engine_data.mesh_registry.getMesh(model.mesh);

        push_constants.model_data =
            model_data_buffer.getDeviceAddress(frame_data) +
            sizeof(StaticModelBuffer) * i;
        push_constants.vertices = mesh->vertex_buffer.device_address;
        push_constants.meshlet_triangles =
            mesh->meshlet_triangles_buffer.device_address;
        push_constants.meshlet_vertices =
            mesh->meshlet_vertices_buffer.device_address;
        push_constants.meshlets = mesh->meshlet_buffer.device_address;

        command_buffer.pushConstants(pipeline, &push_constants, 0);

        command_buffer.draw(mesh->meshlet_count);
    }
}

void StaticMeshRenderer::setCameraData(const VkDeviceAddress camera_data) {
    push_constants.camera_data = camera_data;
}

}  // namespace Graphics