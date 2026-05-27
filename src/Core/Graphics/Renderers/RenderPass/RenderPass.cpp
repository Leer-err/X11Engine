#include "RenderPass.h"

#include <tracy/Tracy.hpp>

#include "BufferBuilder.h"
#include "CameraData.h"
#include "CloudsData.h"
#include "CloudsRenderer.h"
#include "Device.h"
#include "DitheringPass.h"
#include "GraphicsCommunicationManager.h"
#include "OverlayRenderer.h"
#include "StaticModelData.h"

namespace Graphics {

RenderPass::RenderPass(EngineData engine_data)
    : engine_data(engine_data),
      star_renderer(engine_data),
      clouds_renderer(engine_data),
      static_mesh_renderer(engine_data),
      overlay_renderer(engine_data),
      camera_data_buffer(engine_data) {}

void RenderPass::render(const FrameData& frame_data) {
    ZoneScoped;

    auto camera_data_address = camera_data_buffer.getDeviceAddress(frame_data);

    clouds_renderer.setCameraData(camera_data_address);
    star_renderer.setCameraData(camera_data_address);
    static_mesh_renderer.setCameraData(camera_data_address);

    auto& manager = GraphicsCommunicationManager::get();

    TracyVkZone(frame_data.trace_ctx, frame_data.cmd.buffer, "Render pass");

    auto clouds = manager.recieve<CloudsData>();

    if (clouds) {
        clouds_renderer.preRender(frame_data, clouds.value());
    }

    updateCameraBuffer(frame_data);

    beginPass(frame_data);

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

    endPass(frame_data);
}

void RenderPass::beginPass(const FrameData& frame_data) {
    frame_data.cmd.bindRenderEnviroment(frame_data.env);
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

}  // namespace Graphics
