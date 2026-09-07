#include "CloudsRenderer.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <cstring>

#include "Buffer.h"
#include "BufferBuilder.h"
#include "CloudsData.h"
#include "DescriptorSet.h"
#include "Device.h"
#include "EngineData.h"
#include "GraphicsPipelineBuilder.h"
#include "MeshBuilder.h"
#include "RenderWorld.h"
#include "Sampler.h"
#include "TextureBuilder.h"
#include "Vector2.h"
#include "Vector3.h"
#include "VertexFormats.h"

namespace Graphics {

struct Vertex {
    Vector3 vertex_position;
    Vector2 uv;
};

CloudsRenderer::CloudsRenderer(Device& device, const EngineData& engine_data)
    : engine_data(engine_data),
      quad(createScreenQuad(device, engine_data)),
      cloud_plane(createCloudPlane(device, engine_data)),
      clouds_data_buffer(createCloudDataBuffer(device)) {
    clouds_texture = TextureBuilder(VK_FORMAT_R8G8B8A8_UNORM, 512, 512)
                         .isRenderTarget()
                         .isShaderResource()
                         .create(device, engine_data.texture_registry)
                         .getResult();
    engine_data.descriptor_set.addTexture(clouds_texture);
    engine_data.descriptor_set.addSampler(Sampler::linear(device));

    cloud_texture_pipeline =
        GraphicsPipelineBuilder(
            "./Assets/Shaders/Pipelines/Clouds/CloudsTexture.spv",
            "vertex_main",
            "./Assets/Shaders/Pipelines/Clouds/CloudsTexture.spv", "pixel_main")
            .setRenderTargetFormat(VK_FORMAT_R8G8B8A8_UNORM)
            .create(device, engine_data.shader_registry)
            .getResult();
    cloud_pipeline =
        GraphicsPipelineBuilder(
            "./Assets/Shaders/Pipelines/Clouds/Clouds.spv", "vertex_main",
            "./Assets/Shaders/Pipelines/Clouds/Clouds.spv", "pixel_main")
            .writesDepth()
            .create(device, engine_data.shader_registry)
            .getResult();
}

void CloudsRenderer::render(FrameGraph& frame_graph, const RenderWorld& world) {
    // TracyVkZone(frame_data.trace_ctx, frame_data.cmd.buffer, "Clouds");

    clouds_data_buffer.update(world.getCloudsData());

    auto cloud_prepass = GraphicsPass(
        "Cloud bake", cloud_pipeline, [this](GraphicsPassExecution& execution) {
            auto clouds_address = clouds_data_buffer.getDeviceAddress();

            execution.appendData(clouds_address);
            execution.draw(quad);
        });
    cloud_prepass.addColorAttachment(clouds_texture);
    frame_graph.addGraphicsPass(cloud_prepass);

    auto pass = GraphicsPass("Clouds", cloud_pipeline,
                             [this](GraphicsPassExecution& execution) {
                                 push_constants.clouds_address =
                                     clouds_data_buffer.getDeviceAddress();

                                 execution.appendData(push_constants);
                                 execution.draw(cloud_plane);
                             });
    pass.reads(clouds_texture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    auto color_texture = *engine_data.texture_registry.getTexture("Color");
    pass.addColorAttachment(color_texture);
    auto depth_texture = *engine_data.texture_registry.getTexture("Depth");
    pass.setDepthAttachment(depth_texture);
    frame_graph.addGraphicsPass(pass);
}

void CloudsRenderer::setCameraData(VkDeviceAddress camera_data) {
    push_constants.camera_address = camera_data;
}

Buffer CloudsRenderer::createCloudDataBuffer(Device& device) {
    return BufferBuilder(sizeof(CloudsData))
        .isConstantBuffer()
        .isDeviceAddressable()
        .isChained()
        .isCPUWritable(true)
        .create(device)
        .getResult();
}

Mesh CloudsRenderer::createScreenQuad(Device& device,
                                      const EngineData& engine_data) {
    constexpr std::array<Vertex, 4> screen_quad_vertices = {
        Vertex{Vector3(-1, -1, 1), Vector2(0, 0)},
        Vertex{Vector3(-1, 1, 1), Vector2(0, 1)},
        Vertex{Vector3(1, -1, 1), Vector2(0, 1)},
        Vertex{Vector3(1, 1, 1), Vector2(0, 1)}};

    constexpr std::array<uint32_t, 6> screen_quad_indices = {0, 1, 2, 1, 3, 2};

    return MeshBuilder(&screen_quad_vertices[0], sizeof(screen_quad_vertices),
                       &screen_quad_indices[0], sizeof(screen_quad_indices))
        .create(device, engine_data.staging_buffer);
}

Mesh CloudsRenderer::createCloudPlane(Device& device,
                                      const EngineData& engine_data) {
    constexpr std::array<Vertex, 4> cloud_plane_vertex_data = {
        Vertex{Vector3(-1, 0, -1), Vector2(0, 0)},
        Vertex{Vector3(-1, 0, 1), Vector2(0, 1)},
        Vertex{Vector3(1, 0, -1), Vector2(1, 0)},
        Vertex{Vector3(1, 0, 1), Vector2(1, 1)}};

    constexpr std::array<uint32_t, 6> screen_quad_indices = {0, 1, 2, 1, 3, 2};

    return MeshBuilder(cloud_plane_vertex_data.data(),
                       sizeof(cloud_plane_vertex_data),
                       screen_quad_indices.data(), sizeof(screen_quad_indices))
        .create(device, engine_data.staging_buffer);
}

}  // namespace Graphics