#pragma once

#include "BufferedUniform.h"
#include "CameraData.h"
#include "CloudsRenderer.h"
#include "Device.h"
#include "DitheringPass.h"
#include "FrameData.h"
#include "OverlayRenderer.h"
#include "RenderEnviroment.h"
#include "StarRenderer.h"
#include "StaticMeshRenderer.h"
#include "TextureHandle.h"

namespace Graphics {

class RenderPass {
   public:
    RenderPass(Device& device, const EngineData& engine_data);

    void render(const FrameData& frame_data);

   private:
    void beginPass(const FrameData& frame_data);
    void endPass(const FrameData& frame_data);

    void updateCameraBuffer(const FrameData& frame_data);

    void createRenderEnviroment(Device& device);

    EngineData engine_data;

    BufferedUniform<CameraData> camera_data_buffer;

    StarRenderer star_renderer;
    CloudsRenderer clouds_renderer;
    StaticMeshRenderer static_mesh_renderer;
    OverlayRenderer overlay_renderer;

    RenderEnviroment env;
    TextureHandle render_target_handle;
};

}  // namespace Graphics