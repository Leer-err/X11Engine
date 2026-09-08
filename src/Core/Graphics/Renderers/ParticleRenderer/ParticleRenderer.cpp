#include "ParticleRenderer.h"

#include <vulkan/vulkan.h>

#include <set>
#include <tracy/Tracy.hpp>
#include <unordered_set>

#include "BufferBuilder.h"
#include "EngineData.h"
#include "GraphicsPipelineBuilder.h"
#include "Handles.h"
#include "ParticleHandle.h"

namespace Graphics {

ParticleRenderer::ParticleRenderer(Device& device,
                                   const EngineData& engine_data)
    : engine_data(engine_data),
      particle_buffer(createParticleBuffer(device)),
      live_particles_buffer(createLiveParticleBuffer(device)),
      quad(createQuadMesh(engine_data)) {
    pipeline = GraphicsPipelineBuilder(
                   "./Assets/Shaders/Pipelines/Particles/SpriteParticles.spv",
                   "mesh_main",
                   "./Assets/Shaders/Pipelines/Particles/SpriteParticles.spv",
                   "pixel_main")
                   .create(device, engine_data.shader_registry)
                   .getResult();
}

void ParticleRenderer::render(FrameGraph& frame_graph,
                              const RenderWorld& world) {
    ZoneScoped;

    auto particles = world.getParticles();
    if (particles.alive.empty()) return;

    std::vector<ParticleHandle> alive_particles(particles.alive.begin(),
                                                particles.alive.end());

    particle_buffer.update(std::bit_cast<uint8_t*>(particles.particles.data()),
                           sizeof(Particle) * particles.particles.size(), 0);
    live_particles_buffer.update(
        std::bit_cast<uint8_t*>(alive_particles.data()),
        sizeof(ParticleHandle) * alive_particles.size(), 0);

    auto particle_count = particles.alive.size();

    auto pass = GraphicsPass(
        "Particles", pipeline,
        [this, particle_count](GraphicsPassExecution& execution) {
            push_constants.particles_data = particle_buffer.getDeviceAddress();
            push_constants.live_particles_data =
                live_particles_buffer.getDeviceAddress();

            execution.appendData(push_constants);
            execution.draw(quad, particle_count);
        });

    // auto render_target = engine_data.texture_registry.getTexture("Color");
    // pass.addColorAttachment(*render_target);
    // auto depth = engine_data.texture_registry.getTexture("Depth");
    // pass.setDepthAttachment(*depth);
    // for (const auto texture_handle : particle_texture_descriptors) {
    //     auto texture =
    //     *engine_data.texture_registry.getTexture(texture_handle);
    //     pass.reads(texture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    // }

    frame_graph.addGraphicsPass(pass);
}

void ParticleRenderer::setCameraData(VkDeviceAddress data) {
    push_constants.camera_data = data;
}

Mesh ParticleRenderer::createQuadMesh(const EngineData& engine_data) {
    return *engine_data.mesh_registry.getMesh("Quad");
}

Buffer ParticleRenderer::createParticleBuffer(Device& device) {
    return BufferBuilder(sizeof(Particle) * MAX_PARTICLE_COUNT)
        .isConstantBuffer()
        .isDeviceAddressable()
        .isChained()
        .isCPUWritable(true)
        .create(device)
        .getResult();
}

Buffer ParticleRenderer::createLiveParticleBuffer(Device& device) {
    return BufferBuilder(sizeof(ParticleHandle) * MAX_PARTICLE_COUNT)
        .isConstantBuffer()
        .isDeviceAddressable()
        .isChained()
        .isCPUWritable(true)
        .create(device)
        .getResult();
}

}  // namespace Graphics