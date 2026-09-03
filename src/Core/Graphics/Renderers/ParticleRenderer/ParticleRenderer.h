#pragma once

#include <vulkan/vulkan_core.h>

#include "EngineData.h"
#include "FrameGraph.h"
#include "RenderWorld.h"
#include "StagingBuffer.h"

namespace Graphics {

class ParticleRenderer {
    static constexpr size_t MAX_PARTICLE_COUNT = 10000;

    struct PushConstants {
        VkDeviceAddress camera_data;
        VkDeviceAddress particle_positions_data;
        VkDeviceAddress particle_colors_data;
        VkDeviceAddress particle_sizes_data;
        VkDeviceAddress particle_textures_data;
        VkDeviceAddress live_particles;
    };

   public:
    ParticleRenderer(Device& device, const EngineData& engine_data);

    void render(FrameGraph& frame_graph, const RenderWorld& world);

    void setCameraData(VkDeviceAddress camera_data);

   private:
    static Mesh createQuadMesh(const EngineData& engine_data);
    static Buffer createParticlePositionsBuffer(Device& device);
    static Buffer createParticleColorsBuffer(Device& device);
    static Buffer createParticleSizesBuffer(Device& device);
    static Buffer createParticleMaterialsBuffer(Device& device);
    static Buffer createLiveParticlesBuffer(Device& device);

    EngineData engine_data;

    Buffer particle_positions_buffer;
    Buffer particle_colors_buffer;
    Buffer particle_sizes_buffer;
    Buffer particle_materials_buffer;
    Buffer live_particles_buffer;

    GraphicsPipeline pipeline;
    PushConstants push_constants;
    Mesh quad;
};

}  // namespace Graphics