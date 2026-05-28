#pragma once

#include "BufferedUniform.h"
#include "Device.h"
#include "EngineData.h"
#include "FrameData.h"
#include "GraphicsPipeline.h"
#include "TextureHandle.h"

namespace Graphics {

class DitheringPass {
    struct DitheringData {
        std::array<int, 2> camera_dimensions;
        float spread;
        TextureHandle render_target_index;
        uint32_t sampler_index;
    };

   public:
    DitheringPass(Device& device, const EngineData& engine_data);

    void render(const FrameData& frame_data);

   private:
    EngineData engine_data;

    GraphicsPipeline pipeline;
    Mesh quad;

    BufferedUniform<DitheringData> dithering_data_buffer;
};

}  // namespace Graphics