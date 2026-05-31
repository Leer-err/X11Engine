#pragma once

#include <vulkan/vulkan_core.h>

#include "Buffer.h"
#include "BufferedUniform.h"
#include "CloudsData.h"
#include "CommandBuffer.h"
#include "DescriptorSet.h"
#include "EngineData.h"
#include "FrameData.h"
#include "GraphicsPipeline.h"
#include "RenderEnviroment.h"

namespace Graphics {

class CloudsRenderer {
    struct PushConstants {
        VkDeviceAddress camera_address;
        VkDeviceAddress clouds_address;
    };

   public:
    CloudsRenderer(Device& device, const EngineData& engine_data);

    void render(const FrameData& frame_data, const CloudsData& clouds_data);
    void preRender(const FrameData& frame_data, const CloudsData& clouds_data);

    void setCameraData(VkDeviceAddress camera_data);

   private:
    EngineData engine_data;

    Mesh quad;
    Mesh cloud_plane;

    GraphicsPipeline cloud_texture_pipeline;
    GraphicsPipeline cloud_pipeline;
    RenderEnviroment env;

    Texture clouds_texture;
    BufferedUniform<CloudsData> clouds_data_buffer;

    PushConstants push_constants;
};

}  // namespace Graphics