#include "RenderPass.h"

#include <vulkan/vulkan.h>

#include <array>
#include <tracy/Tracy.hpp>

#include "AppConfig.h"
#include "BufferBuilder.h"
#include "CameraData.h"
#include "CloudsRenderer.h"
#include "Device.h"
#include "FrameGraph.h"
#include "OverlayRenderer.h"
#include "PostProcessingPass.h"
#include "RenderEnviroment.h"
#include "RenderWorld.h"
#include "TextureBuilder.h"

namespace Graphics {

RenderPass::RenderPass(Device& device, const EngineData& engine_data)
    : engine_data(engine_data),
      camera_data_buffer(createCameraBuffer(device)),
      star_renderer(device, engine_data),
      static_mesh_renderer(device, engine_data),
      //   clouds_renderer(device, engine_data),
      particle_renderer(device, engine_data),
      mesh_particle_renderer(device, engine_data),
      overlay_renderer(),
      post_processing_pass(device, engine_data) {
    createRenderEnviroment(device);
}

Texture RenderPass::render(const FrameData& frame_data, FrameGraph& frame_graph,
                           const RenderWorld& world) {
    ZoneScoped;

    auto camera_data_address = camera_data_buffer.getDeviceAddress();

    star_renderer.setCameraData(camera_data_address);
    static_mesh_renderer.setCameraData(camera_data_address);
    particle_renderer.setCameraData(camera_data_address);
    mesh_particle_renderer.setCameraData(camera_data_address);

    // TracyVkZone(frame_data.trace_ctx, frame_data.cmd.buffer, "Render pass");

    updateCameraBuffer(world);

    star_renderer.render(frame_graph, world);
    static_mesh_renderer.render(frame_graph, world);
    // clouds_renderer.render(frame_graph, world);
    particle_renderer.render(frame_graph, world);
    mesh_particle_renderer.render(frame_graph, world);

    postProcessing(frame_graph, world);

    return final_image;
}

void RenderPass::updateCameraBuffer(const RenderWorld& world) {
    auto camera_data = world.getCameraData();

    camera_data_buffer.update(camera_data);
}

void RenderPass::createRenderEnviroment(Device& device) {
    auto config = Config::App::get().getGraphicsConfig();

    auto width = config.render_width;
    auto height = config.render_height;

    auto device_properties = device.getDeviceProperties();

    render_target_texture =
        TextureBuilder(VK_FORMAT_R8G8B8A8_SRGB, config.render_width,
                       config.render_height)
            .setName("Color")
            .isCopySource()
            .isRenderTarget()
            .isShaderResource()
            .create(device, engine_data.texture_registry)
            .getResult();
    engine_data.descriptor_set.addTexture(render_target_texture);

    depth_stencil_texture =
        TextureBuilder(device_properties.depth_format, config.render_width,
                       config.render_height)
            .setName("Depth")
            .isDepthStencil()
            .create(device, engine_data.texture_registry)
            .getResult();

    final_image = TextureBuilder(VK_FORMAT_R8G8B8A8_SRGB, config.render_width,
                                 config.render_height)
                      .setName("RenderResult")
                      .isCopySource()
                      .isRenderTarget()
                      .isShaderResource()
                      .create(device, engine_data.texture_registry)
                      .getResult();
}

void RenderPass::postProcessing(FrameGraph& frame_graph,
                                const RenderWorld& world) {
    post_processing_pass.render(render_target_texture, frame_graph, world);

    // frame_data.cmd.bindRenderEnviroment(post_process_env);

    // overlay_renderer.render(frame_data);

    // frame_data.cmd.unbindRenderEnviroment();
}

Buffer RenderPass::createCameraBuffer(Device& device) {
    auto builder = BufferBuilder(sizeof(CameraData))
                       .isConstantBuffer()
                       .isDeviceAddressable()
                       .isCPUWritable(true)
                       .isChained();

    return builder.create(device).getResult();
}

}  // namespace Graphics
