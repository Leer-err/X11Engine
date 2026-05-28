#include "RenderPass.h"

#include <vulkan/vulkan_core.h>

#include <array>
#include <tracy/Tracy.hpp>

#include "AppConfig.h"
#include "BufferBuilder.h"
#include "CameraData.h"
#include "CloudsData.h"
#include "CloudsRenderer.h"
#include "Device.h"
#include "DitheringPass.h"
#include "GraphicsCommunicationManager.h"
#include "ImageBuilder.h"
#include "OverlayRenderer.h"
#include "RenderEnviroment.h"
#include "StaticModelData.h"

namespace Graphics {

RenderPass::RenderPass(Device& device, const EngineData& engine_data)
    : engine_data(engine_data),
      camera_data_buffer(device),
      star_renderer(device, engine_data),
      clouds_renderer(device, engine_data),
      static_mesh_renderer(device, engine_data),
      dithering_pass(device, engine_data),
      overlay_renderer() {
    createRenderEnviroment(device);
}

Image& RenderPass::render(const FrameData& frame_data) {
    ZoneScoped;

    auto camera_data_address = camera_data_buffer.getDeviceAddress(frame_data);

    clouds_renderer.setCameraData(camera_data_address);
    star_renderer.setCameraData(camera_data_address);
    static_mesh_renderer.setCameraData(camera_data_address);

    auto& manager = GraphicsCommunicationManager::get();

    // TracyVkZone(frame_data.trace_ctx, frame_data.cmd.buffer, "Render pass");

    auto clouds = manager.recieve<CloudsData>();

    if (clouds) {
        clouds_renderer.preRender(frame_data, clouds.value());
    }

    updateCameraBuffer(frame_data);

    std::array<VkImageMemoryBarrier2, 2> barriers;
    barriers[0] = render_target_image.createBarrier(
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_2_NONE,
        VK_ACCESS_2_NONE, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);
    barriers[1] = depth_stencil_image.createBarrier(
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        VK_PIPELINE_STAGE_2_NONE, VK_ACCESS_2_NONE,
        VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
        VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
            VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);

    frame_data.cmd.barrier(barriers);

    frame_data.cmd.bindRenderEnviroment(env);

    auto stars = manager.recieve<StarsData>();
    if (stars) {
        star_renderer.render(frame_data, *stars);
    }
    if (clouds) {
        clouds_renderer.render(frame_data, clouds.value());
    }

    while (auto model = manager.recieve<StaticModelData>()) {
        static_mesh_renderer.queueMeshForRender(frame_data, model.value());
    }
    static_mesh_renderer.render(frame_data);

    overlay_renderer.render(frame_data);

    frame_data.cmd.unbindRenderEnviroment();

    prepareForPostProcess(frame_data);

    frame_data.cmd.bindRenderEnviroment(post_process_env);
    dithering_pass.render(render_target_handle, frame_data);
    frame_data.cmd.unbindRenderEnviroment();

    return final_image;
}

void RenderPass::prepareForPostProcess(const FrameData& frame_data) {
    std::array<VkImageMemoryBarrier2, 2> barriers;
    barriers[0] = render_target_image.createBarrier(
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_SHADER_READ_BIT);
    barriers[1] = final_image.createBarrier(
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_2_NONE,
        VK_ACCESS_2_NONE, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);

    frame_data.cmd.barrier(barriers);
}

void RenderPass::beginPass(const FrameData& frame_data) {
    frame_data.cmd.bindRenderEnviroment(env);
}

void RenderPass::endPass(const FrameData& frame_data) {
    frame_data.cmd.unbindRenderEnviroment();
}

void RenderPass::updateCameraBuffer(const FrameData& frame_data) {
    auto camera_data =
        GraphicsCommunicationManager::get().recieve<CameraData>();

    if (!camera_data) return;

    camera_data_buffer.update(frame_data, *camera_data);
}

void RenderPass::createRenderEnviroment(Device& device) {
    auto config = Config::App::get().getGraphicsConfig();

    auto width = config.render_width;
    auto height = config.render_height;

    render_target_image =
        ImageBuilder(VK_FORMAT_R8G8B8A8_SRGB, config.render_width,
                     config.render_height)
            .isCopySource()
            .isRenderTarget()
            .isShaderResource()
            .create(device)
            .getResult();

    depth_stencil_image =
        ImageBuilder(VK_FORMAT_D24_UNORM_S8_UINT, config.render_width,
                     config.render_height)
            .isDepthStencil()
            .create(device)
            .getResult();

    env = RenderEnviroment{};
    env.width = width;
    env.height = height;
    env.render_target = device.createTextureView(render_target_image);
    env.clear_render_target = true;
    env.render_target_clear_value = VkClearValue{.color = {0, 0, 0, 1}};
    env.depth_stencil = device.createDepthStencil(depth_stencil_image);
    env.clear_depth_stencil = true;
    env.clear_depth = 1;
    env.clear_stencil = 0;

    render_target_handle =
        engine_data.descriptor_set.addImage(env.render_target);

    final_image = ImageBuilder(VK_FORMAT_R8G8B8A8_SRGB, config.render_width,
                               config.render_height)
                      .isCopySource()
                      .isRenderTarget()
                      .isShaderResource()
                      .create(device)
                      .getResult();
    post_process_env = RenderEnviroment{};
    post_process_env.width = width;
    post_process_env.height = height;
    post_process_env.render_target = device.createTextureView(final_image);
    env.clear_render_target = true;
}

}  // namespace Graphics
