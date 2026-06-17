#pragma once

#include "Buffer.h"
#include "CameraData.h"
#include "Device.h"
#include "FrameData.h"
#include "FrameGraph.h"
#include "OverlayRenderer.h"
#include "PostProcessingPass.h"
#include "RenderEnviroment.h"
#include "RenderWorld.h"
#include "StarRenderer.h"
#include "StaticMeshRenderer.h"
#include "Texture.h"

namespace Graphics {

class RenderPass {
   public:
    RenderPass(Device& device, const EngineData& engine_data);

    Texture render(const FrameData& frame_data, FrameGraph& frame_graph,
                   const RenderWorld& world);

   private:
    void postProcessing(FrameGraph& frame_graph, const RenderWorld& world);

    void updateCameraBuffer(const RenderWorld& world);

    void createRenderEnviroment(Device& device);

    static Buffer createCameraBuffer(Device& device);

    EngineData engine_data;

    Buffer camera_data_buffer;

    StarRenderer star_renderer;
    StaticMeshRenderer static_mesh_renderer;
    OverlayRenderer overlay_renderer;

    PostProcessingPass post_processing_pass;

    RenderEnviroment env;
    Texture render_target_texture;
    Texture depth_stencil_texture;

    RenderEnviroment post_process_env;
    Texture final_image;
};

}  // namespace Graphics