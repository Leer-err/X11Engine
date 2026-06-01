#pragma once

#include "BufferedUniform.h"
#include "EngineData.h"
#include "FrameData.h"

namespace Graphics {

class PostProcessingPass {
    struct PostProcessingData {
        std::array<uint32_t, 2> camera_dimensions;
        uint32_t render_target_index;
        uint32_t sampler_index;

        // dithering
        float spread;

        uint32_t color_count;
    };

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