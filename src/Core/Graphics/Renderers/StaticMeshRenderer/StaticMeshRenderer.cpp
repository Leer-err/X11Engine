#include "StaticMeshRenderer.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <bit>

#include "BufferBuilder.h"
#include "Device.h"
#include "FrameData.h"
#include "FrameGraph.h"
#include "GraphicsPipelineBuilder.h"
#include "Matrix.h"
#include "RenderWorld.h"
#include "Sampler.h"

namespace Graphics {

StaticMeshRenderer::StaticMeshRenderer(Device& device,
                                       const EngineData& engine_data)
    : engine_data(engine_data), model_data_buffer(createModelBuffer(device)) {
    pipeline = GraphicsPipelineBuilder(
                   "./Assets/Shaders/StaticModel/StaticModel.spv", "mesh_main",
                   "./Assets/Shaders/StaticModel/StaticModel.spv", "pixel_main")
                   .writesDepth()
                   .create(device, engine_data.shader_registry)
                   .getResult();

    sampler_index =
        engine_data.descriptor_set.addSampler(Sampler::linear(device));
}

void StaticMeshRenderer::render(FrameGraph& frame_graph,
                                const RenderWorld& world) {
    // TracyVkZone(frame_data.trace_ctx, frame_data.cmd.buffer, "Static mesh");

    auto buffer_ptr =
        std::bit_cast<ModelBuffer*>(model_data_buffer.getHostAddress());
    auto objects = world.getOpaqueObjects();

    auto pass = GraphicsPass(
        pipeline, [this, objects](GraphicsPassExecution& execution) {
            auto buffer_address = model_data_buffer.getDeviceAddress();

            for (int i = 0; i < objects.size(); i++) {
                const auto& model = objects[i];
                auto mesh = engine_data.mesh_registry.getMesh(model.mesh);

                push_constants.model_data =
                    buffer_address + sizeof(ModelData) * i;
                execution.appendData(push_constants);
                execution.draw(*mesh);
            }
        });

    for (int i = 0; i < objects.size(); i++) {
        const auto& model = objects[i];

        auto& model_data = (*buffer_ptr)[i];
        model_data.model = Matrix::translation(model.position);
        model_data.albedo_descriptor =
            *engine_data.descriptor_set.getIndex(model.albedo);
        model_data.albedo_sampler = sampler_index;

        pass.reads(*engine_data.texture_registry.getTexture(model.albedo),
                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
    pass.addColorAttachment(*engine_data.texture_registry.getTexture("Color"));
    pass.setDepthAttachment(*engine_data.texture_registry.getTexture("Depth"));

    frame_graph.addGraphicsPass(pass);
}

void StaticMeshRenderer::setCameraData(const VkDeviceAddress camera_data) {
    push_constants.camera_data = camera_data;
}

Buffer StaticMeshRenderer::createModelBuffer(Device& device) {
    return BufferBuilder(sizeof(ModelBuffer))
        .isConstantBuffer()
        .isDeviceAddressable()
        .isCPUWritable(true)
        .isChained()
        .create(device)
        .getResult();
}

}  // namespace Graphics