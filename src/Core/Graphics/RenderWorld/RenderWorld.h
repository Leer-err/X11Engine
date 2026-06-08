#pragma once

#include <vector>

#include "CameraData.h"
#include "Matrix.h"
#include "MeshHandle.h"
#include "PostProcessingData.h"
#include "StarsData.h"
#include "TextureHandle.h"

namespace Graphics {

struct RenderObject {
    Matrix world_matrix;
    MeshHandle mesh;

    TextureHandle albedo;
};

using RenderObjectHandle = uint32_t;

class RenderWorld {
   public:
    RenderWorld();

    void setCameraData(const CameraData& camera_data);
    CameraData getCameraData() const;
    void setStarsData(const StarsData& data);
    StarsData getStarsData() const;
    void setPostProcessingData(const PostProcessingData& data);
    PostProcessingData getPostProcessingData() const;

    RenderObjectHandle addRenderObject(const Matrix& world_matrix,
                                       const MeshHandle& mesh_handle,
                                       const TextureHandle& texture_handle);
    void updateRenderObject(const RenderObjectHandle& handle,
                            const Matrix& world_matrix,
                            const MeshHandle& mesh_handle,
                            const TextureHandle& texture_handle);
    const std::vector<RenderObject>& getRenderObjects() const;

   private:
    std::vector<RenderObject> opaque_objects;
    RenderObjectHandle next_handle;

    StarsData stars_data;
    PostProcessingData post_processing_data;
    CameraData camera_data;
};

}  // namespace Graphics