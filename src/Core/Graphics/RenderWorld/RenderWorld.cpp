#include "RenderWorld.h"

namespace Graphics {

RenderWorld::RenderWorld() : next_handle(0) {}

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

RenderObjectHandle RenderWorld::addRenderObject(
    const Matrix& world_matrix, const MeshHandle& mesh_handle,
    const TextureHandle& texture_handle) {
    opaque_objects.emplace_back(world_matrix, mesh_handle, texture_handle);

    return next_handle++;
}

void RenderWorld::updateRenderObject(const RenderObjectHandle& handle,
                                     const Matrix& world_matrix,
                                     const MeshHandle& mesh_handle,
                                     const TextureHandle& texture_handle) {
    auto& object = opaque_objects[handle];
    object.world_matrix = world_matrix;
    object.mesh = mesh_handle;
    object.albedo = texture_handle;
}

const std::vector<RenderObject>& RenderWorld::getRenderObjects() const {
    return opaque_objects;
}

}  // namespace Graphics