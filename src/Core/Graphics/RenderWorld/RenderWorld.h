#pragma once

#include <span>
#include <vector>

#include "CameraData.h"
#include "CloudsData.h"
#include "EffectDescription.h"
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
    void setCloudsData(const CloudsData& data);
    CloudsData getCloudsData() const;

    OpaqueObjectHandle addOpaqueObject(const OpaqueRenderObjectData& data);
    OpaqueRenderObjectData& getOpaqueObject(const OpaqueObjectHandle& handle);
    std::span<const OpaqueRenderObjectData> getOpaqueObjects() const;

    void addEffect(const EffectDescription& description);

    void update(float delta_time);

    const ParticleBatchState& getParticles() const;

   private:
    std::vector<OpaqueRenderObjectData> opaque_objects;
    OpaqueObjectHandle next_handle;

    VFXWorld vfx_world;

    StarsData stars_data;
    CloudsData clouds_data;
    PostProcessingData post_processing_data;
    CameraData camera_data;
};

}  // namespace Graphics