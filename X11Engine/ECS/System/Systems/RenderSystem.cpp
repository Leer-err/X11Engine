#include "RenderSystem.h"

#include <math.h>

#include <future>
#include <iostream>
#include <stack>
#include <vector>

#include "ECS/Component/ComponentManager.h"
#include "ECS/Component/Components/CameraComponent.h"
#include "ECS/Component/Components/PointLightComponent.h"
#include "ECS/Component/Components/RenderComponent.h"
#include "ECS/Component/Components/TransformComponent.h"
#include "Graphics/Graphics.h"
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

bool TestAABBCollision(const AABB& box, const Scene::Transform& transform,
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

void RenderSystem::Update() {
    vector<future<void>> completed_tasks;

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
    Scene::Node* worldNode = Scene::get()->GetWorldNode();

    stack<Scene::Node*> nodes;
    nodes.push(worldNode);

    for (Scene::Node* node; nodes.size() != 0;) {
        node = nodes.top();
        nodes.pop();

        Model* model = node->GetModel();
        for (const auto& child : node->GetChildren()) nodes.push(child);
        if (model == nullptr) continue;
        Graphics::get()->SetWorldMatrix(node->GetWorldMatrix());
        Graphics::get()->UpdatePerModelBuffers();

        vector<matrix> finalboneMatrices;
        for (int i = 0; i < finalboneMatrices.size(); i++) {
            finalboneMatrices[i] = model->bones[i].offsetMatrix;
        }

        for (const auto& mesh : model->meshes) {
            if (TestAABBCollision(mesh.boundingBox, node->GetTransform(),
                                  viewFrustum)) {
                completed_tasks.emplace_back(TaskManager::get()->submit(
                    &Graphics::Draw, Graphics::get(), std::cref(mesh),
                    std::cref(*(model->materials[mesh.materialIndex]))));
            }
        }
        for (const auto& child : node->GetChildren()) {
            nodes.push(child);
        }
    }

    for (const auto& task : completed_tasks) {
        task.wait();
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
