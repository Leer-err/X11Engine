#pragma once

#include "EngineData.h"
#include "FrameData.h"
#include "RenderWorld.h"
#include "StagingBuffer.h"

namespace Graphics {

class ParticleRenderer {
   public:
    ParticleRenderer(Device& device, const EngineData& engine_data);

    void render(const FrameData& frame_data, const RenderWorld& world);

   private:
    static Mesh createQuadMesh(Device& device, StagingBuffer& staging_buffer);

    EngineData engine_data;

    GraphicsPipeline pipeline;
    Mesh quad;

    PostProcessingData data;
};

}  // namespace Graphics