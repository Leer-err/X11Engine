#pragma once

#include "EngineData.h"
#include "FrameData.h"
#include "RenderWorld.h"

namespace Graphics {

class ParticleRenderer {
   public:
    ParticleRenderer(Device& device, const EngineData& engine_data);

    void render(const FrameData& frame_data, const RenderWorld& world);

   private:
    EngineData engine_data;

    GraphicsPipeline pipeline;
    Mesh quad;

    PostProcessingData data;
};

}  // namespace Graphics