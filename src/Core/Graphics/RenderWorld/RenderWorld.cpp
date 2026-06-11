#include "RenderWorld.h"

#include "ParticlePool.h"

namespace Graphics {

RenderWorld::RenderWorld()
    : next_handle(0), particle_pool(1000), effects({Effect(particle_pool)}) {}

void RenderWorld::setCameraData(const CameraData& data) {
    this->camera_data = data;
}

CameraData RenderWorld::getCameraData() const { return camera_data; }

void RenderWorld::setStarsData(const StarsData& data) {
    this->stars_data = data;
}

StarsData RenderWorld::getStarsData() const { return stars_data; }

void RenderWorld::setPostProcessingData(const PostProcessingData& data) {
    this->post_processing_data = data;
}

PostProcessingData RenderWorld::getPostProcessingData() const {
    return post_processing_data;
}

OpaqueObjectHandle RenderWorld::addOpaqueObject(
    const OpaqueRenderObjectData& data) {
    opaque_objects.emplace_back(data);

    return next_handle++;
}

OpaqueRenderObjectData& RenderWorld::getOpaqueObject(
    const OpaqueObjectHandle& handle) {
    return opaque_objects.at(handle);
}

std::span<const OpaqueRenderObjectData> RenderWorld::getOpaqueObjects() const {
    return opaque_objects;
}

const ParticleBatchState& RenderWorld::getParticles() const {
    return particle_pool.getBatch();
}

}  // namespace Graphics