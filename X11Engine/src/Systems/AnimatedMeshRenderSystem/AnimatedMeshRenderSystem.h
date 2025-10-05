#ifndef ANIMATED_MESH_RENDER_SYSTEM_H
#define ANIMATED_MESH_RENDER_SYSTEM_H

#include <memory>
#include <vector>

#include "Animation.h"
#include "AnimationRegistry.h"
#include "IResourceFactory.h"
#include "IShaderReader.h"
#include "ISystem.h"
#include "Logger.h"
#include "Model.h"
#include "World.h"

class AnimatedMeshRenderSystem : public ISystem {
    struct TransformedSkeleton {
        std::vector<Matrix> skeleton;
        std::vector<bool> is_animated;
    };

   public:
    AnimatedMeshRenderSystem(
        std::shared_ptr<IResourceFactory> factory,
        std::shared_ptr<IShaderReader> shader_reader,
        std::shared_ptr<AnimationRegistry> animation_registry);

    bool prepare() override;

    void update(World& world, float delta_time) override;

   private:
    std::vector<Matrix> compute_bones(
        const Skeleton& skeleton, const Mesh& mesh,
        const TransformedSkeleton& transformed_skeleton);

    TransformedSkeleton compute_skeleton(const Skeleton& skeleton,
                                         const Animation& animation,
                                         float time);

    std::shared_ptr<IConstantBuffer> camera_buffer;
    std::shared_ptr<IConstantBuffer> world_matrix_buffer;
    std::shared_ptr<IStructuredBuffer> bone_buffer;
    std::shared_ptr<IStructuredBuffer> default_bone_buffer;

    std::shared_ptr<IPixelShader> pixel_shader;
    std::shared_ptr<IVertexShader> vertex_shader;
    std::shared_ptr<IShaderReader> shader_reader;

    std::shared_ptr<IRenderContext> context;
    std::shared_ptr<IResourceFactory> factory;

    std::shared_ptr<AnimationRegistry> animation_registry;

    Logger logger;
};

#endif  // ANIMATED_MESH_RENDER_SYSTEM_H