#pragma once

#include <span>
#include <vector>

#include "CameraData.h"
#include "OpaqueRenderObjectData.h"
#include "ParticlePool.h"
#include "PostProcessingData.h"
#include "StarsData.h"
#include "VFXWorld.h"

namespace Graphics {

using OpaqueObjectHandle = uint32_t;

class RenderWorld {
   public:
    RenderWorld();

    void setCameraData(const CameraData& data);
    CameraData getCameraData() const;
    void setStarsData(const StarsData& data);
    StarsData getStarsData() const;
    void setPostProcessingData(const PostProcessingData& data);
    PostProcessingData getPostProcessingData() const;

    OpaqueObjectHandle addOpaqueObject(const OpaqueRenderObjectData& data);
    OpaqueRenderObjectData& getOpaqueObject(const OpaqueObjectHandle& handle);
    std::span<const OpaqueRenderObjectData> getOpaqueObjects() const;

    const ParticleBatchState& getParticles() const;

   private:
    std::vector<OpaqueRenderObjectData> opaque_objects;
    OpaqueObjectHandle next_handle;

    ParticlePool particle_pool;
    std::vector<Effect> effects;

    StarsData stars_data;
    PostProcessingData post_processing_data;
    CameraData camera_data;
};

}  // namespace Graphics