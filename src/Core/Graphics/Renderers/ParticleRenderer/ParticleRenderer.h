#pragma once

#include <vulkan/vulkan_core.h>

#include "EngineData.h"
#include "FrameGraph.h"
#include "RenderWorld.h"

namespace Graphics {

class ParticleRenderer {
    static constexpr size_t MAX_PARTICLE_COUNT = 10000;

    struct PushConstants {
        VkDeviceAddress camera_data;
        VkDeviceAddress particles_data;
        VkDeviceAddress live_particles_data;
    };

   public:
    ParticleRenderer(Device& device, const EngineData& engine_data);

    void render(FrameGraph& frame_graph, const RenderWorld& world);

    void setCameraData(VkDeviceAddress camera_data);

   private:
    static Mesh createQuadMesh(const EngineData& engine_data);
    static Buffer createParticleBuffer(Device& device);
    static Buffer createLiveParticleBuffer(Device& device);

    EngineData engine_data;

    Buffer particle_buffer;
    Buffer live_particles_buffer;

    GraphicsPipeline pipeline;
    PushConstants push_constants;
    Mesh quad;
};

}  // namespace Graphics