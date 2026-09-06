#include "RenderWorld.h"

#include <span>

#include "VFXWorld.h"

namespace Graphics {

RenderWorld::RenderWorld() : next_handle(0), vfx_world(10000) {}

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

void RenderWorld::setCloudsData(const CloudsData& data) { clouds_data = data; }

CloudsData RenderWorld::getCloudsData() const { return clouds_data; }

OpaqueObjectHandle RenderWorld::addOpaqueObject(const RenderObjectData& data) {
    opaque_objects.emplace_back(data);

    return next_handle++;
}

RenderObjectData& RenderWorld::getOpaqueObject(
    const OpaqueObjectHandle& handle) {
    return opaque_objects.at(handle);
}

std::span<const RenderObjectData> RenderWorld::getOpaqueObjects() const {
    return opaque_objects;
}

void RenderWorld::addEffect(const EffectDescription& description) {
    vfx_world.addEffect(description);
}

VFXWorld::ParticleBatch RenderWorld::getParticles() const {
    return vfx_world.getParticles();
}

void RenderWorld::update(float delta_time) { vfx_world.update(delta_time); }

}  // namespace Graphics