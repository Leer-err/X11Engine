#pragma once

#include "BufferedUniform.h"
#include "CameraData.h"
#include "CloudsRenderer.h"
#include "Device.h"
#include "FrameData.h"
#include "OverlayRenderer.h"
#include "PostProcessingPass.h"
#include "RenderEnviroment.h"
#include "StarRenderer.h"
#include "StaticMeshRenderer.h"
#include "TextureHandle.h"

namespace Graphics {

class RenderPass {
   public:
    RenderPass(Device& device, const EngineData& engine_data);

    Image& render(const FrameData& frame_data);

   private:
    void postProcessing(const FrameData& frame_data);

    void updateCameraBuffer(const FrameData& frame_data);

    void createRenderEnviroment(Device& device);

    EngineData engine_data;

    BufferedUniform<CameraData> camera_data_buffer;

    StarRenderer star_renderer;
    CloudsRenderer clouds_renderer;
    StaticMeshRenderer static_mesh_renderer;
    OverlayRenderer overlay_renderer;

    PostProcessingPass post_processing_pass;

    RenderEnviroment env;
    Image render_target_image;
    Image depth_stencil_image;
    TextureHandle render_target_handle;

    RenderEnviroment post_process_env;
    Image final_image;
};

}  // namespace Graphics