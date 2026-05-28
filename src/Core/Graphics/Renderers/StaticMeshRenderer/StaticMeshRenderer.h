#pragma once

#include <vulkan/vulkan.h>

#include <array>
#include <cstddef>

#include "BufferedUniform.h"
#include "EngineData.h"
#include "FrameData.h"
#include "Matrix.h"
#include "StaticModelData.h"

namespace Graphics {

class StaticMeshRenderer {
    static constexpr auto MAX_STATIC_MESHES_PER_DRAW = 1000000;
    struct StaticModelBuffer {
        Matrix model;

        TextureHandle albedo_descriptor;
    };

    struct PushConstants {
        VkDeviceAddress camera_data;
        VkDeviceAddress model_data;
    };

   public:
    StaticMeshRenderer(Device& device, const EngineData& engine_data);

    void queueMeshForRender(const FrameData& frame_data,
                            const StaticModelData& model_data);
    void render(const FrameData& frame_data);
    void setCameraData(VkDeviceAddress camera_data);

   private:
    size_t next_object_index;

    EngineData engine_data;

    GraphicsPipeline pipeline;

    std::array<StaticModelData, MAX_STATIC_MESHES_PER_DRAW> models;
    BufferedUniform<std::array<StaticModelBuffer, MAX_STATIC_MESHES_PER_DRAW>>
        model_data_buffer;

    PushConstants push_constants;
};

}  // namespace Graphics