#include "StarRenderer.h"

#include <vulkan/vulkan.h>

#include <array>
#include <cstring>

#include "BufferBuilder.h"
#include "Device.h"
#include "EngineData.h"
#include "FrameGraph.h"
#include "GraphicsMesh.h"
#include "GraphicsPipelineBuilder.h"
#include "MeshBuilder.h"
#include "RenderWorld.h"
#include "StarsData.h"
#include "Texture.h"
#include "Vector2.h"
#include "Vector3.h"

namespace Graphics {

StarRenderer::StarRenderer(Device& device, const EngineData& engine_data)
    : engine_data(engine_data),
      quad(createSkyMesh(engine_data)),
      stars_data_buffer(createStarsBuffer(device)) {
    pipeline = GraphicsPipelineBuilder(
                   "./Assets/Shaders/Pipelines/Stars/Stars.spv", "mesh_main",
                   "./Assets/Shaders/Pipelines/Stars/Stars.spv", "pixel_main")
                   .create(device, engine_data.shader_registry)
                   .getResult();
}

void StarRenderer::render(FrameGraph& frame_graph, const RenderWorld& world) {
    auto pass = GraphicsPass(
        "Stars", pipeline, [this, &world](GraphicsPassExecution& execution) {
            auto stars_data = world.getStarsData();
            stars_data_buffer.update(stars_data);
            push_constants.stars_data = stars_data_buffer.getDeviceAddress();

            execution.appendData(push_constants);

            execution.draw(quad);
        });

    // auto color_attachment = engine_data.texture_registry.getTexture("Color");
    // pass.addColorAttachment(color_attachment.value(), {});
    // auto depth_attachment = engine_data.texture_registry.getTexture("Depth");
    // pass.setDepthAttachment(
    //     depth_attachment.value(),
    //     VkClearValue{.depthStencil = {.depth = 1, .stencil = 0}});

    frame_graph.addGraphicsPass(pass);
}

void StarRenderer::setCameraData(VkDeviceAddress camera_data) {
    push_constants.camera_data = camera_data;
}

Buffer StarRenderer::createStarsBuffer(Device& device) {
    return BufferBuilder(sizeof(StarsData))
        .isConstantBuffer()
        .isChained()
        .isDeviceAddressable()
        .isCPUWritable(true)
        .create(device)
        .getResult();
}

Mesh StarRenderer::createSkyMesh(const EngineData& engine_data) {
    return engine_data.mesh_registry.getMesh("SkySphere").value();
}

}  // namespace Graphics