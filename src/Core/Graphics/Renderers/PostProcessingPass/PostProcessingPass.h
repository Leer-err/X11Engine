#pragma once

#include "BufferedUniform.h"
#include "EngineData.h"
#include "FrameData.h"
#include "PostProcessingData.h"

namespace Graphics {

class PostProcessingPass {
   public:
    PostProcessingPass(Device& device, const EngineData& engine_data);

    void render(const Texture& input_image, const FrameData& frame_data);

   private:
    EngineData engine_data;

    GraphicsPipeline pipeline;
    Mesh quad;

    BufferedUniform<PostProcessingData> dithering_data_buffer;
    PostProcessingData data;
};

}  // namespace Graphics