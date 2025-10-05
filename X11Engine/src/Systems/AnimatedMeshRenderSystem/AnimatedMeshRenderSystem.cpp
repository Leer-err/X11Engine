#include "AnimatedMeshRenderSystem.h"

#include <cstddef>
#include <tracy/Tracy.hpp>
#include <vector>

#include "AnimatedMesh.h"
#include "LoggerFactory.h"
#include "Matrix.h"
#include "Model.h"
#include "PlayerComponent.h"
#include "PositionComponents.h"
#include "Quaternion.h"
#include "StaticProjectionCamera.h"
#include "Vector3.h"
#include "World.h"

AnimatedMeshRenderSystem::AnimatedMeshRenderSystem(
    std::shared_ptr<IResourceFactory> factory,
    std::shared_ptr<IShaderReader> shader_reader,
    std::shared_ptr<AnimationRegistry> animation_registry)
    : context(factory->getContext()),
      factory(factory),
      shader_reader(shader_reader),
      animation_registry(animation_registry),
      logger(LoggerFactory::getLogger("AnimatedMeshRenderSystem")) {}

bool AnimatedMeshRenderSystem::prepare() {
    constexpr char PIXEL_SHADER_FILE[] =
        "E:"
        "\\repos\\X11Engine\\X11Engine\\src\\Shaders\\AnimatedShaders\\PixelSha"
        "der.hlsl";
    constexpr char VERTEX_SHADER_FILE[] =
        "E:"
        "\\repos\\X11Engine\\X11Engine\\src\\Shaders\\AnimatedShaders\\VertexSh"
        "ader.hlsl";

    std::shared_ptr<IShaderBlob> pixel_shader_source =
        shader_reader->readPixelShader(PIXEL_SHADER_FILE);
    pixel_shader = factory->createPixelShader(pixel_shader_source.get());

    if (pixel_shader == nullptr) {
        logger.error("Pixel shader was not loaded");
        return false;
    }

    std::shared_ptr<IShaderBlob> vertex_shader_source =
        shader_reader->readVertexShader(VERTEX_SHADER_FILE);
    vertex_shader = factory->createVertexShader(vertex_shader_source.get());

    if (vertex_shader == nullptr) {
        logger.error("Pixel shader was not loaded");
        return false;
    }

    camera_buffer =
        factory->createConstantBuffer(nullptr, sizeof(Matrix), true);
    world_matrix_buffer =
        factory->createConstantBuffer(nullptr, sizeof(Matrix), true);

    bone_buffer =
        factory->createStructuredBuffer(nullptr, 1024, sizeof(Matrix), true);

    auto default_bones = std::vector<Matrix>(1024, Matrix::identity());
    default_bone_buffer = factory->createStructuredBuffer(
        (char*)default_bones.data(), 1024, sizeof(Matrix), true);

    return true;
}

void AnimatedMeshRenderSystem::update(World& world, float delta_time) {
    ZoneScoped;
    context->setPixelShader(pixel_shader);
    context->setVertexShader(vertex_shader);

    std::vector<Entity> players = world.query().with<Player>().execute();
    std::vector<Entity> cameras = world.query()
                                      .childOf(players[0])
                                      .with<StaticProjectionCamera>()
                                      .with<GlobalMatrix>()
                                      .execute();
    auto camera = (const ICamera*)cameras[0].get<StaticProjectionCamera>();
    auto camera_transform = cameras[0].get<GlobalMatrix>();

    auto camera_translation = camera_transform->matrix.getTranslation();
    auto camera_orientation = camera_transform->matrix.getRotation();
    auto camera_projection_matrix = camera->getProjectionMatrix();

    Matrix camera_view_matrix = Matrix::view(
        camera_translation, Vector3(0, 0, 1).rotate(camera_orientation),
        Vector3(0, 1, 0).rotate(camera_orientation));

    Matrix camera_matrix = camera_view_matrix * camera_projection_matrix;

    camera_buffer->update((char*)&camera_matrix, sizeof(Matrix), 0);
    context->bindConstantBuffer(ShaderType::Vertex, camera_buffer, 0);

    std::vector<Entity> entities =
        world.query().with<AnimatedMesh>().with<GlobalMatrix>().execute();
    for (auto& entity : entities) {
        const Model& model = entity.get<AnimatedMesh>()->model;
        const GlobalMatrix* transform = entity.get<GlobalMatrix>();

        const Material& material = model.material;

        auto current_animation = entity.get<CurrentAnimation>();

        const Animation* animation =
            animation_registry->get(current_animation->animation_name);

        TransformedSkeleton transformed_skeleton = compute_skeleton(
            model.skeleton, *animation, current_animation->current_time);

        context->bindTexture(ShaderType::Pixel, material.albedo_texture, 0);

        world_matrix_buffer->update((char*)&transform->matrix, sizeof(Matrix),
                                    0);
        context->bindConstantBuffer(ShaderType::Vertex, world_matrix_buffer, 1);

        for (const Mesh& mesh : model.meshes) {
            std::vector<Matrix> bones =
                compute_bones(model.skeleton, mesh, transformed_skeleton);

            bone_buffer->update((char*)bones.data(),
                                bones.size() * sizeof(Matrix), 0);

            context->bindStructuredBuffer(ShaderType::Vertex, bone_buffer, 0);

            context->setVertexBuffer(mesh.vertex_buffer);
            context->setIndexBuffer(mesh.index_buffer);

            context->draw(mesh.index_buffer->getIndexCount());
        }

        float new_time = current_animation->current_time + delta_time;
        if (new_time * animation->ticks_per_second > 100) new_time = 0;

        entity.set<CurrentAnimation>(
            {new_time, current_animation->animation_name});
    }
}

AnimatedMeshRenderSystem::TransformedSkeleton
AnimatedMeshRenderSystem::compute_skeleton(const Skeleton& skeleton,
                                           const Animation& animation,
                                           float time) {
    std::vector<Matrix> transformed_skeleton;
    std::vector<bool> animated_flags;

    size_t node_count = skeleton.bone_names.size();
    transformed_skeleton.resize(node_count);
    animated_flags.resize(node_count);

    for (size_t node_index = 0; node_index < node_count; node_index++) {
        const std::string& node_name = skeleton.bone_names.at(node_index);

        auto positions_it = animation.positions.find(node_name);
        auto rotations_it = animation.rotations.find(node_name);
        auto scalings_it = animation.scalings.find(node_name);

        bool is_animated = false;
        Matrix bone_transform = Matrix::identity();

        float animation_timestamp = time * animation.ticks_per_second;
        size_t key_index = animation_timestamp;
        float blend_factor = animation_timestamp - key_index;

        if (scalings_it != animation.scalings.end()) {
            const std::vector<ScalingKey>& scalings = scalings_it->second;

            if (key_index + 1 < scalings.size()) {
                Vector3 last_scale = scalings[key_index].scaling;
                Vector3 next_scale = scalings[key_index + 1].scaling;

                Vector3 scale =
                    Vector3::lerp(last_scale, next_scale, blend_factor);
                bone_transform = bone_transform * Matrix::scale(scale);
                is_animated = true;
            }
        }

        if (rotations_it != animation.rotations.end()) {
            const std::vector<RotationKey>& rotations = rotations_it->second;

            if (key_index + 1 < rotations.size()) {
                Quaternion last_rotation = rotations[key_index].rotation;
                Quaternion next_rotation = rotations[key_index + 1].rotation;

                Quaternion rotation = Quaternion::slerp(
                    last_rotation, next_rotation, blend_factor);
                bone_transform = bone_transform * Matrix::rotation(rotation);
                is_animated = true;
            }
        }

        if (positions_it != animation.positions.end()) {
            const std::vector<PositionKey>& positions = positions_it->second;

            if (key_index + 1 < positions.size()) {
                Vector3 last_position = positions[key_index].position;
                Vector3 next_position = positions[key_index + 1].position;

                Vector3 position =
                    Vector3::lerp(last_position, next_position, blend_factor);
                bone_transform = bone_transform * Matrix::translation(position);
                is_animated = true;
            }
        }

        if (!is_animated) {
            bone_transform = skeleton.transforms[node_index];
        }

        size_t parent_index = skeleton.parents[node_index];
        if (parent_index != node_index) {
            bone_transform =
                bone_transform * transformed_skeleton[parent_index];
        }

        transformed_skeleton[node_index] = std::move(bone_transform);
        animated_flags[node_index] = is_animated;
    }

    for (size_t node_index = 0; node_index < node_count; node_index++) {
        transformed_skeleton[node_index] =
            skeleton.inverse_transforms[node_index] *
            transformed_skeleton[node_index];
    }

    return {std::move(transformed_skeleton), std::move(animated_flags)};
}

std::vector<Matrix> AnimatedMeshRenderSystem::compute_bones(
    const Skeleton& skeleton, const Mesh& mesh,
    const TransformedSkeleton& transformed_skeleton) {
    size_t bone_count = mesh.bones.size();

    std::vector<Matrix> final_bones;
    final_bones.reserve(bone_count);

    for (auto& bone : mesh.bones) {
        final_bones.emplace_back(
            std::move(transformed_skeleton.skeleton[bone.skeleton_node_index]));
    }

    return final_bones;
}