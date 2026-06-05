#pragma once

#include <vector>

#include "CameraData.h"
#include "Matrix.h"
#include "MeshHandle.h"
#include "PostProcessingData.h"
#include "StarsData.h"
#include "TextureHandle.h"

struct RenderObject {
    Matrix world_matrix;
    MeshHandle mesh;

    TextureHandle albedo;
};

using RenderObjectHandle = uint32_t;

class RenderWorld {
   public:
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
    RenderObject getRenderObject(const RenderObjectHandle& handle) const;

   private:
    std::vector<RenderObject> opaque_objects;

    StarsData stars_data;
    PostProcessingData post_processing_data;
    CameraData camera_data;
};