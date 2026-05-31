#include "CloudsRenderer.h"

#include <vulkan/vulkan.h>

#include <cstring>

#include "CloudsData.h"
#include "DescriptorSet.h"
#include "Device.h"
#include "EngineData.h"
#include "GraphicsPipelineBuilder.h"
#include "MeshBuilder.h"
#include "Sampler.h"
#include "TextureBuilder.h"
#include "Vector2.h"
#include "Vector3.h"

namespace Graphics {

struct Vertex {
    Vector3 vertex_position;
    Vector2 uv;
};

CloudsRenderer::CloudsRenderer(Device& device, const EngineData& engine_data)
    : engine_data(engine_data), env({}), clouds_data_buffer(device) {
    constexpr Vertex cloud_plane_vertex_data[] = {
        {Vector3(-1, 0, -1), Vector2(0, 0)},
        {Vector3(-1, 0, 1), Vector2(0, 1)},
        {Vector3(1, 0, -1), Vector2(1, 0)},
        {Vector3(1, 0, 1), Vector2(1, 1)}};

    constexpr Vector3 screen_quad_vertices[] = {
        Vector3(-1, -1, 1), Vector3(-1, 1, 1), Vector3(1, -1, 1),
        Vector3(1, 1, 1)};

    constexpr uint32_t screen_quad_indices[] = {0, 1, 2, 1, 3, 2};

    cloud_plane =
        MeshBuilder(cloud_plane_vertex_data, sizeof(cloud_plane_vertex_data),
                    screen_quad_indices, sizeof(screen_quad_indices))
            .create(device, engine_data.staging_buffer);
    quad = MeshBuilder(&screen_quad_vertices[0], sizeof(screen_quad_vertices),
                       &screen_quad_indices[0], sizeof(screen_quad_indices))
               .create(device, engine_data.staging_buffer);

    clouds_texture = TextureBuilder(VK_FORMAT_R8G8B8A8_UNORM, 512, 512)
                         .isRenderTarget()
                         .isShaderResource()
                         .create(device, engine_data.texture_registry)
                         .getResult();
    engine_data.descriptor_set.addTexture(clouds_texture);
    engine_data.descriptor_set.addSampler(Sampler::linear(device));

    env.width = 512;
    env.height = 512;
    env.render_target = device.createTextureView(clouds_texture.getState());
    env.clear_render_target = true;
    env.render_target_clear_value.color = {0, 0, 0, 0};

    cloud_texture_pipeline =
        GraphicsPipelineBuilder(
            "./Assets/Shaders/Clouds/CloudsTexture.spv", "vertex_main",
            "./Assets/Shaders/Clouds/CloudsTexture.spv", "pixel_main")
            .setRenderTargetFormat(VK_FORMAT_R8G8B8A8_UNORM)
            .create(device, engine_data.shader_registry)
            .getResult();
    cloud_pipeline = GraphicsPipelineBuilder(
                         "./Assets/Shaders/Clouds/Clouds.spv", "vertex_main",
                         "./Assets/Shaders/Clouds/Clouds.spv", "pixel_main")
                         .create(device, engine_data.shader_registry)
                         .getResult();
}

void CloudsRenderer::render(const FrameData& frame_data,
                            const CloudsData& clouds_data) {
    // TracyVkZone(frame_data.trace_ctx, frame_data.cmd.buffer, "Clouds");

    auto command_buffer = frame_data.cmd;

    push_constants.clouds_address =
        clouds_data_buffer.getDeviceAddress(frame_data);

    command_buffer.setPipeline(cloud_pipeline);
    command_buffer.bindDescriptorSet(cloud_pipeline,
                                     engine_data.descriptor_set);

    command_buffer.pushConstants(cloud_pipeline, &push_constants);

    command_buffer.draw(cloud_plane);
}

void CloudsRenderer::preRender(const FrameData& frame_data,
                               const CloudsData& clouds_data) {
    // TracyVkZone(frame_data.trace_ctx, frame_data.cmd.buffer,
    //             "Cloud texture bake");

    clouds_data_buffer.update(frame_data, clouds_data);

    auto command_buffer = frame_data.cmd;

    auto render_target_barrier = clouds_texture.createBarrier(
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_2_NONE,
        VK_ACCESS_2_NONE, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);

    command_buffer.barrier(&render_target_barrier, 1, nullptr, 0);

    command_buffer.bindRenderEnviroment(env);

    command_buffer.setPipeline(cloud_texture_pipeline);
    command_buffer.bindDescriptorSet(cloud_texture_pipeline,
                                     engine_data.descriptor_set);

    auto clouds_device_address =
        clouds_data_buffer.getDeviceAddress(frame_data);
    command_buffer.pushConstants(cloud_texture_pipeline,
                                 &clouds_device_address);

    command_buffer.draw(quad);

    command_buffer.unbindRenderEnviroment();

    auto cloud_texture_barrier = clouds_texture.createBarrier(
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_SHADER_READ_BIT);
    command_buffer.barrier(&cloud_texture_barrier, 1, nullptr, 0);
}

void CloudsRenderer::setCameraData(VkDeviceAddress camera_data) {
    push_constants.camera_address = camera_data;
}

}  // namespace Graphics