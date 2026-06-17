#pragma once

#include <vulkan/vulkan.h>

#include <array>
#include <cstddef>

#include "Buffer.h"
#include "EngineData.h"
#include "FrameData.h"
#include "FrameGraph.h"
#include "Matrix.h"
#include "RenderWorld.h"

namespace Graphics {

class StaticMeshRenderer {
    static constexpr auto MAX_STATIC_MESHES_PER_DRAW = 1000000;

    struct ModelData {
        Matrix model;

        uint32_t albedo_descriptor;
        uint32_t albedo_sampler;
    };

    using ModelBuffer = std::array<ModelData, MAX_STATIC_MESHES_PER_DRAW>;

    struct PushConstants {
        VkDeviceAddress camera_data;
        VkDeviceAddress model_data;
    };

   public:
    StaticMeshRenderer(Device& device, const EngineData& engine_data);

    void render(const FrameData& frame_data, FrameGraph& frame_graph,
                const RenderWorld& world);
    void setCameraData(VkDeviceAddress camera_data);

   private:
    static Buffer createModelBuffer(Device& device);
    EngineData engine_data;

    GraphicsPipeline pipeline;

    Buffer model_data_buffer;

    PushConstants push_constants;
    uint32_t sampler_index;
};

}  // namespace Graphics