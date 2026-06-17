#pragma once

#include "EngineData.h"
#include "FrameData.h"
#include "FrameGraph.h"
#include "GraphicsPipeline.h"
#include "Mesh.h"
#include "RenderWorld/RenderWorld.h"
#include "StarsData.h"

namespace Graphics {

class StarRenderer {
    struct PushConstants {
        VkDeviceAddress camera_data;
        VkDeviceAddress stars_data;
    };

   public:
    StarRenderer(Device& device, const EngineData& engine_data);

    void render(FrameGraph& frame_graph, const RenderWorld& world);

    void setCameraData(VkDeviceAddress camera_data);

   private:
    static Buffer createStarsBuffer(Device& device);
    static Mesh createScreenQuad(Device& device, const EngineData& engine_data);

    EngineData engine_data;

    Mesh quad;

    GraphicsPipeline pipeline;

    Buffer stars_data_buffer;

    PushConstants push_constants;
};

}  // namespace Graphics