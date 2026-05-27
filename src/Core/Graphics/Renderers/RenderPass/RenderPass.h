#pragma once

#include "BufferedUniform.h"
#include "CameraData.h"
#include "CloudsRenderer.h"
#include "DitheringPass.h"
#include "FrameData.h"
#include "OverlayRenderer.h"
#include "StarRenderer.h"
#include "StaticMeshRenderer.h"

namespace Graphics {

class RenderPass {
   public:
    RenderPass(EngineData engine_data);

    void render(const FrameData& frame_data);

   private:
    void beginPass(const FrameData& frame_data);
    void endPass(const FrameData& frame_data);

    void updateCameraBuffer(const FrameData& frame_data);

    EngineData engine_data;

    BufferedUniform<CameraData> camera_data_buffer;

    StarRenderer star_renderer;
    CloudsRenderer clouds_renderer;
    StaticMeshRenderer static_mesh_renderer;
    OverlayRenderer overlay_renderer;

    DitheringPass dithering;
};

}  // namespace Graphics