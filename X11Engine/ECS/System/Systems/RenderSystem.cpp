#include "RenderSystem.h"

#include <math.h>

#include <future>
#include <iostream>
#include <map>
#include <stack>
#include <vector>

#include "ECS/Component/ComponentManager.h"
#include "ECS/Component/Components/AnimationComponent.h"
#include "ECS/Component/Components/CameraComponent.h"
#include "ECS/Component/Components/PointLightComponent.h"
#include "ECS/Component/Components/RenderComponent.h"
#include "ECS/Component/Components/TransformComponent.h"
#include "Graphics/Graphics.h"
#include "MaterialRegistry/MaterialRegistry.h"
#include "Scene/Scene.h"
#include "TaskManager/TaskManager.h"

using std::abs;
using std::future;
using std::stack;
using std::vector;

bool IsForwardPlan(const AABB& box, const Frustum::Plan& plan) {
    float r = box.extents.x * abs(plan.normal.x) +
              box.extents.y * abs(plan.normal.y) +
              box.extents.z * abs(plan.normal.z);

    return -r <= (dot(plan.normal, box.center) - plan.distance);
}

bool TestAABBCollision(const AABB& box, const Transform& transform,
                       const Frustum& viewFrustum) {
    vector3 globalCenter = box.center.transform(transform.worldMatrix);

    vector3 right = transform.GetRight() * box.extents.x;
    vector3 up = transform.GetUp() * box.extents.y;
    vector3 forward = transform.GetForward() * box.extents.z;

    float extentX = abs(dot(LOCAL_RIGHT, right)) + abs(dot(LOCAL_RIGHT, up)) +
                    abs(dot(LOCAL_RIGHT, forward));

    float extentY = abs(dot(LOCAL_UP, right)) + abs(dot(LOCAL_UP, up)) +
                    abs(dot(LOCAL_UP, forward));

    float extentZ = abs(dot(LOCAL_FORWARD, right)) +
                    abs(dot(LOCAL_FORWARD, up)) +
                    abs(dot(LOCAL_FORWARD, forward));

    AABB globalBox(globalCenter, extentX, extentY, extentZ);

    return IsForwardPlan(globalBox, viewFrustum.leftPlan) &&
           IsForwardPlan(globalBox, viewFrustum.rightPlan) &&
           IsForwardPlan(globalBox, viewFrustum.bottomPlan) &&
           IsForwardPlan(globalBox, viewFrustum.topPlan) &&
           IsForwardPlan(globalBox, viewFrustum.farPlan) &&
           IsForwardPlan(globalBox, viewFrustum.nearPlan);
}

void RenderSystem::PreUpdate() { Graphics::get()->Clear(); }

void RenderSystem::Update(float delta) {
    CameraComponent* camera = cameraEntity.GetComponent<CameraComponent>();
    TransformComponent* cameraTransform =
        cameraEntity.GetComponent<TransformComponent>();
    Graphics::get()->SetViewMatrix(
        LookToMatrix(
            cameraTransform->GetPosition(),
            camera->camera.forward.rotate(cameraTransform->GetRotation()),
            camera->camera.up.rotate(cameraTransform->GetRotation())),
        cameraTransform->GetPosition());

    Frustum viewFrustum(camera->camera,
                        cameraTransform->sceneNode->GetTransform());

    auto& light = ECS::ComponentManager::get()->begin<PointLightComponent>();
    for (int i = 0;
         i < MAX_POINT_LIGHTS &&
         light != ECS::ComponentManager::get()->end<PointLightComponent>();
         i++, ++light) {
        vector3 pos = static_cast<EntityId>(light->GetOwner())
                          .GetComponent<TransformComponent>()
                          ->GetPosition();
        Graphics::get()->SetPointLight(light->light, pos);
    }
    Graphics::get()->UpdatePerFrameBuffers();
    Graphics::get()->DrawSkybox();

    Scene::get()->WaitForUpdate();

    for (auto iter = ECS::ComponentManager::get()->begin<RenderComponent>();
         iter != ECS::ComponentManager::get()->end<RenderComponent>(); ++iter) {
        const auto& entity = iter->GetOwner();
        const auto& model = iter->model;
        const auto& sceneNode = ECS::ComponentManager::get()
                                    ->GetComponent<TransformComponent>(entity)
                                    ->sceneNode;
        const auto& animationComponent =
            ECS::ComponentManager::get()->GetComponent<AnimationComponent>(
                entity);

        if (animationComponent != nullptr) {
            vector<matrix> boneMatrices(model->skeleton.offsetMatrices.size());
            vector<matrix> finalboneMatrices(
                model->skeleton.offsetMatrices.size());

            const auto& animation = animationComponent->animation;
            const auto& animationTime = animationComponent->time;

            for (int i = 0; i < model->skeleton.offsetMatrices.size(); i++) {
                int parentIndex = model->skeleton.parents[i];

                matrix worldMatrix = IdentityMatrix();

                auto scaleKey = animation->GetScaleKey(i, animationTime);
                auto rotationKey = animation->GetRotationKey(i, animationTime);
                auto positionKey = animation->GetPositionKey(i, animationTime);

                worldMatrix = worldMatrix * ScalingMatrix(scaleKey);
                worldMatrix = worldMatrix * RotationMatrix(rotationKey);
                worldMatrix = worldMatrix * TranslationMatrix(positionKey);

                if (parentIndex == INVALID_PARENT) {
                    boneMatrices[i] = worldMatrix;
                } else {
                    boneMatrices[i] = worldMatrix * boneMatrices[parentIndex];
                }

                if (animation->IsBoneAnimated(i)) {
                    finalboneMatrices[i] =
                        (model->skeleton.offsetMatrices[i] * boneMatrices[i])
                            .Transpose();
                } else {
                    finalboneMatrices[i] = finalboneMatrices[parentIndex];
                }
            }

            Graphics::get()->SetBoneData(finalboneMatrices);
        }

        matrix worldMatrix = sceneNode->GetWorldMatrix();
        Graphics::get()->SetWorldMatrix(worldMatrix);
        Graphics::get()->UpdatePerModelBuffers();

        for (const auto& mesh : model->meshes) {
            if (!TestAABBCollision(mesh.boundingBox, sceneNode->GetTransform(),
                                   viewFrustum)) {
                continue;
            }
            const auto& material_index = model->materials[mesh.materialIndex];
            const auto& material =
                *MaterialRegistry::get()->GetMaterial(material_index);
            Graphics::get()->Draw(mesh, material);
        }
    }
}

void RenderSystem::PostUpdate() { Graphics::get()->Present(); }

void RenderSystem::SetCamera(const EntityId camera) {
    cameraEntity = camera;
    CameraComponent* cameraComponent =
        cameraEntity.GetComponent<CameraComponent>();
    if (cameraComponent == nullptr)
        FatalError(L"Entity used to set camera don't have camera component");
    else {
        Graphics::get()->SetProjectionMatrix(
            cameraComponent->camera.projection);
        Graphics::get()->UpdatePerWindowBuffers();
    }
}
