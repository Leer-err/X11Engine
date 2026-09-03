#include "ParticleRenderer.h"

#include <vulkan/vulkan.h>

#include <array>
#include <bit>
#include <cstdint>
#include <tracy/Tracy.hpp>
#include <vector>

#include "BufferBuilder.h"
#include "EngineData.h"
#include "GraphicsPipelineBuilder.h"
#include "MeshBuilder.h"
#include "ParticleHandle.h"
#include "TextureHandle.h"
#include "Vector3.h"

namespace Graphics {

ParticleRenderer::ParticleRenderer(Device& device,
                                   const EngineData& engine_data)
    : engine_data(engine_data),
      particle_positions_buffer(createParticlePositionsBuffer(device)),
      particle_colors_buffer(createParticleColorsBuffer(device)),
      particle_sizes_buffer(createParticleSizesBuffer(device)),
      particle_materials_buffer(createParticleMaterialsBuffer(device)),
      live_particles_buffer(createLiveParticlesBuffer(device)),
      quad(createQuadMesh(engine_data)) {
    pipeline = GraphicsPipelineBuilder(
                   "./Assets/Shaders/Particles/Particles.spv", "mesh_main",
                   "./Assets/Shaders/Particles/Particles.spv", "pixel_main")
                   .create(device, engine_data.shader_registry)
                   .getResult();

    push_constants.particle_positions_data =
        particle_positions_buffer.getDeviceAddress();
    push_constants.particle_colors_data =
        particle_colors_buffer.getDeviceAddress();
    push_constants.particle_sizes_data =
        particle_sizes_buffer.getDeviceAddress();
    push_constants.particle_textures_data =
        particle_materials_buffer.getDeviceAddress();
    push_constants.live_particles = live_particles_buffer.getDeviceAddress();
}

void ParticleRenderer::render(FrameGraph& frame_graph,
                              const RenderWorld& world) {
    ZoneScoped;

    auto& particles = world.getParticles();
    auto& particle_positions = particles.positions;
    auto& particle_colors = particles.colors;
    auto& particle_sizes = particles.sizes;
    particle_positions_buffer.update(
        std::bit_cast<uint8_t*>(particle_positions.data()),
        particle_positions.size() * sizeof(Vector3), 0);
    particle_sizes_buffer.update(std::bit_cast<uint8_t*>(particle_sizes.data()),
                                 particle_sizes.size() * sizeof(Vector3), 0);
    particle_colors_buffer.update(
        std::bit_cast<uint8_t*>(particle_colors.data()),
        particle_colors.size() * sizeof(Vector4), 0);

    auto& particle_materials = particles.textures;
    std::vector<uint32_t> particle_texture_descriptors;
    particle_texture_descriptors.reserve(particle_materials.size());
    for (auto& texture : particle_materials) {
        particle_texture_descriptors.push_back(
            *engine_data.descriptor_set.getIndex(texture));
    }
    particle_materials_buffer.update(
        std::bit_cast<uint8_t*>(particle_texture_descriptors.data()),
        particle_texture_descriptors.size() * sizeof(uint32_t), 0);

    auto alive_particles = std::vector<ParticleHandle>();
    for (int i = 0; i < MAX_PARTICLE_COUNT; i++) {
        if (particles.alive_flags[i]) alive_particles.push_back(i);
    }
    live_particles_buffer.update(
        std::bit_cast<uint8_t*>(alive_particles.data()),
        alive_particles.size() * sizeof(ParticleHandle), 0);

    auto particle_count = alive_particles.size();

    auto pass =
        GraphicsPass("Particles", pipeline,
                     [this, particle_count](GraphicsPassExecution& execution) {
                         execution.appendData(push_constants);
                         execution.draw(quad, particle_count);
                     });

    auto render_target = engine_data.texture_registry.getTexture("Color");
    pass.addColorAttachment(*render_target);
    auto depth = engine_data.texture_registry.getTexture("Depth");
    pass.setDepthAttachment(*depth);
    for (const auto texture_handle : particle_texture_descriptors) {
        auto texture = *engine_data.texture_registry.getTexture(texture_handle);
        pass.reads(texture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    frame_graph.addGraphicsPass(pass);
}

void ParticleRenderer::setCameraData(VkDeviceAddress data) {
    push_constants.camera_data = data;
}

Mesh ParticleRenderer::createQuadMesh(const EngineData& engine_data) {
    return *engine_data.mesh_registry.getMesh("Quad");
}

Buffer ParticleRenderer::createParticlePositionsBuffer(Device& device) {
    return BufferBuilder(sizeof(Vector3) * MAX_PARTICLE_COUNT)
        .isConstantBuffer()
        .isDeviceAddressable()
        .isChained()
        .isCPUWritable(true)
        .create(device)
        .getResult();
}

Buffer ParticleRenderer::createParticleColorsBuffer(Device& device) {
    return BufferBuilder(sizeof(Vector4) * MAX_PARTICLE_COUNT)
        .isConstantBuffer()
        .isDeviceAddressable()
        .isChained()
        .isCPUWritable(true)
        .create(device)
        .getResult();
}

Buffer ParticleRenderer::createParticleSizesBuffer(Device& device) {
    return BufferBuilder(sizeof(Vector3) * MAX_PARTICLE_COUNT)
        .isConstantBuffer()
        .isDeviceAddressable()
        .isChained()
        .isCPUWritable(true)
        .create(device)
        .getResult();
}

Buffer ParticleRenderer::createParticleMaterialsBuffer(Device& device) {
    return BufferBuilder(sizeof(uint32_t) * MAX_PARTICLE_COUNT)
        .isConstantBuffer()
        .isDeviceAddressable()
        .isChained()
        .isCPUWritable(true)
        .create(device)
        .getResult();
}

Buffer ParticleRenderer::createLiveParticlesBuffer(Device& device) {
    return BufferBuilder(sizeof(ParticleHandle) * MAX_PARTICLE_COUNT)
        .isConstantBuffer()
        .isDeviceAddressable()
        .isChained()
        .isCPUWritable(true)
        .create(device)
        .getResult();
}

}  // namespace Graphics