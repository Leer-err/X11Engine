#include "ParticleRenderer.h"

#include "EngineData.h"

namespace Graphics {

ParticleRenderer::ParticleRenderer(Device& device,
                                   const EngineData& engine_data)
    : engine_data(engine_data) {}

void ParticleRenderer::render(const FrameData& frame_data,
                              const RenderWorld& world) {
    auto& particles = world.getParticles();
}

}  // namespace Graphics