#include "MeshParticleRenderer.h"

#include <vulkan/vulkan.h>

#include <set>
#include <tracy/Tracy.hpp>

#include "BufferBuilder.h"
#include "EngineData.h"
#include "GraphicsPipelineBuilder.h"
#include "Handles.h"
#include "ParticleHandle.h"


namespace Graphics {

MeshParticleRenderer::MeshParticleRenderer(Device& device,
                                           const EngineData& engine_data)
    : engine_data(engine_data), particle_buffer(createParticleBuffer(device)) {
    pipeline = GraphicsPipelineBuilder(
                   "./Assets/Shaders/Pipelines/Particles/MeshParticles.spv",
                   "mesh_main",
                   "./Assets/Shaders/Pipelines/Particles/MeshParticles.spv",
                   "pixel_main")
                   .create(device, engine_data.shader_registry)
                   .getResult();
}

void MeshParticleRenderer::render(FrameGraph& frame_graph,
                                  const RenderWorld& world) {
    ZoneScoped;

    auto particles = world.getMeshParticles();
    if (particles.alive.empty()) return;

    particle_buffer.update(std::bit_cast<uint8_t*>(particles.particles.data()),
                           sizeof(MeshParticle) * particles.particles.size(),
                           0);

    for (int i = 0; i < particles.particles.size(); i++) {
        const auto& particle = particles.particles[i];

        auto pass = GraphicsPass(
            "MeshParticles", pipeline,
            [this, particle, i](GraphicsPassExecution& execution) {
                push_constants.particles_data =
                    particle_buffer.getDeviceAddress() +
                    i * sizeof(MeshParticle);

                execution.appendData(push_constants);
                auto mesh = engine_data.mesh_registry.getMesh(particle.mesh);
                execution.draw(*mesh);
            });

        // auto render_target =
        // engine_data.texture_registry.getTexture("Color");
        // pass.addColorAttachment(*render_target);
        // auto depth = engine_data.texture_registry.getTexture("Depth");
        // pass.setDepthAttachment(*depth);
        // auto texture =
        //     *engine_data.texture_registry.getTexture(particle.texture);
        // pass.reads(texture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        frame_graph.addGraphicsPass(pass);
    }
}

void MeshParticleRenderer::setCameraData(VkDeviceAddress data) {
    push_constants.camera_data = data;
}

Mesh MeshParticleRenderer::createQuadMesh(const EngineData& engine_data) {
    return *engine_data.mesh_registry.getMesh("Quad");
}

Buffer MeshParticleRenderer::createParticleBuffer(Device& device) {
    return BufferBuilder(sizeof(MeshParticle) * MAX_PARTICLE_COUNT)
        .isConstantBuffer()
        .isDeviceAddressable()
        .isChained()
        .isCPUWritable(true)
        .create(device)
        .getResult();
}

}  // namespace Graphics