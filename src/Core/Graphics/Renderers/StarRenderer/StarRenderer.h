#pragma once

#include "BufferedUniform.h"
#include "EngineData.h"
#include "FrameData.h"
#include "GraphicsPipeline.h"
#include "Mesh.h"
#include "StarsData.h"

namespace Graphics {

class StarRenderer {
    struct PushConstants {
        VkDeviceAddress camera_data;
        VkDeviceAddress stars_data;
    };

   public:
    StarRenderer(Device& device, const EngineData& engine_data);

    void render(const FrameData& frame_data, const StarsData& stars_data);

    void setCameraData(VkDeviceAddress camera_data);

   private:
    EngineData engine_data;

    Mesh quad;

    GraphicsPipeline pipeline;

    BufferedUniform<StarsData> stars_data_buffer;

    PushConstants push_constants;
};

}  // namespace Graphics