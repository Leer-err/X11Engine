#include "ParticleRenderer.h"

#include "EngineData.h"
#include "MeshBuilder.h"

namespace Graphics {

ParticleRenderer::ParticleRenderer(Device& device,
                                   const EngineData& engine_data)
    : engine_data(engine_data),
      quad(createQuadMesh(device, engine_data.staging_buffer)) {}

void ParticleRenderer::render(const FrameData& frame_data,
                              const RenderWorld& world) {
    auto& particles = world.getParticles();
}

Mesh ParticleRenderer::createQuadMesh(Device& device,
                                      StagingBuffer& staging_buffer) {
    constexpr std::array<Vertex, 4> quad_vertices = {
        Vertex{Vector3(-0.5, -0.5, 0), Vector2(0, 0)},
        Vertex{Vector3(-0.5, 0.5, 0), Vector2(0, 1)},
        Vertex{Vector3(0.5, -0.5, 0), Vector2(0, 1)},
        Vertex{Vector3(0.5, 0.5, 0), Vector2(0, 1)}};

    constexpr std::array<uint32_t, 6> quad_indices = {0, 1, 2, 1, 3, 2};

    return MeshBuilder(&quad_vertices[0], sizeof(quad_vertices),
                       &quad_indices[0], sizeof(quad_indices))
        .create(device, staging_buffer);
}

}  // namespace Graphics