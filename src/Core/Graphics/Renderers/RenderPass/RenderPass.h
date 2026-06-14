#pragma once

#include "BufferedUniform.h"
#include "CameraData.h"
#include "Device.h"
#include "FrameData.h"
#include "OverlayRenderer.h"
#include "PostProcessingPass.h"
#include "RenderEnviroment.h"
#include "RenderWorld/RenderWorld.h"
#include "StarRenderer.h"
#include "StaticMeshRenderer.h"
#include "Texture.h"

namespace Graphics {

class RenderPass {
   public:
    RenderPass(Device& device, const EngineData& engine_data);

    Texture render(const FrameData& frame_data, const RenderWorld& world);

   private:
    void postProcessing(const FrameData& frame_data, const RenderWorld& world);

    void updateCameraBuffer(const FrameData& frame_data,
                            const RenderWorld& world);

    void createRenderEnviroment(Device& device);

    EngineData engine_data;

    BufferedUniform<CameraData> camera_data_buffer;

    // StarRenderer star_renderer;
    StaticMeshRenderer static_mesh_renderer;
    // OverlayRenderer overlay_renderer;

    PostProcessingPass post_processing_pass;

    RenderEnviroment env;
    Texture render_target_texture;
    Texture depth_stencil_texture;

    RenderEnviroment post_process_env;
    Texture final_image;
};

}  // namespace Graphics