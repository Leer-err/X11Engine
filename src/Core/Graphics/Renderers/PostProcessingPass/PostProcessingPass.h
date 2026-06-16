#pragma once

#include "Buffer.h"
#include "EngineData.h"
#include "FrameData.h"
#include "FrameGraph.h"
#include "PostProcessingData.h"
#include "RenderWorld.h"

namespace Graphics {

class PostProcessingPass {
    struct PostProcessingShaderData {
        std::array<uint32_t, 2> camera_dimensions;
        uint32_t render_target_index;
        uint32_t sampler_index;

        // dithering
        float spread;

        uint32_t color_count;
    };

   public:
    PostProcessingPass(Device& device, const EngineData& engine_data);

    void render(const Texture& input_image, const FrameData& frame_data,
                FrameGraph& frame_graph, const RenderWorld& world);

   private:
    EngineData engine_data;

    GraphicsPipeline pipeline;
    Mesh quad;

    Buffer dithering_data_buffer;
    PostProcessingData data;
};

}  // namespace Graphics