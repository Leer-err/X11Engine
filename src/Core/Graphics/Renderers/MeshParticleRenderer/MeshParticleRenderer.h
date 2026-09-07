#pragma once

#include <vulkan/vulkan_core.h>

#include "EngineData.h"
#include "FrameGraph.h"
#include "ParticleHandle.h"
#include "RenderWorld.h"

namespace Graphics {

class MeshParticleRenderer {
    static constexpr size_t MAX_PARTICLE_COUNT = 10000;

    struct PushConstants {
        VkDeviceAddress camera_data;
        VkDeviceAddress particles_data;
        ParticleHandle particle_index;
    };

   public:
    MeshParticleRenderer(Device& device, const EngineData& engine_data);

    void render(FrameGraph& frame_graph, const RenderWorld& world);

    void setCameraData(VkDeviceAddress camera_data);

   private:
    static Mesh createQuadMesh(const EngineData& engine_data);
    static Buffer createParticleBuffer(Device& device);
    static Buffer createLiveParticleBuffer(Device& device);

    EngineData engine_data;

    Buffer particle_buffer;

    GraphicsPipeline pipeline;
    PushConstants push_constants;
};

}  // namespace Graphics