#include "RenderSystem.h"

#include <future>
#include <vector>

#include "ECS/Component/ComponentManager.h"
#include "ECS/Component/Components/CameraComponent.h"
#include "ECS/Component/Components/PointLightComponent.h"
#include "ECS/Component/Components/RenderComponent.h"
#include "ECS/Component/Components/TransformComponent.h"
#include "ECS/Entity/EntityManager.h"
#include "Graphics/Graphics.h"
#include "Scene/Scene.h"
#include "TaskManager/TaskManager.h"

using std::future;
using std::vector;

void RenderSystem::PreUpdate() { Graphics::get()->Clear(); }

void RenderSystem::Update() {
    vector<future<void>> completed_tasks;

    Scene::get().WaitForUpdate();

    EntityId camera = ECS::ComponentManager::get()
                          ->begin<CameraComponent>()
                          .Get()
                          ->GetOwner();
    TransformComponent* cameraTransform =
        camera.GetComponent<TransformComponent>();
    Graphics::get()->SetViewMatrix(cameraTransform->GetRotation(),
                                   cameraTransform->GetPosition());
    Graphics::get()->DrawSkybox();

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

    for (auto& mesh = ECS::ComponentManager::get()->begin<RenderComponent>();
         mesh != ECS::ComponentManager::get()->end<RenderComponent>(); ++mesh) {
        EntityId entity = mesh->GetOwner();
        const TransformComponent* pos =
            entity.GetComponent<TransformComponent>();
        matrix world = pos->GetWorldMatrix();
        Graphics::get()->SetWorldMatrix(world);
        Graphics::get()->UpdatePerModelBuffers();
        completed_tasks.emplace_back(TaskManager::get()->submit(
            &Graphics::Draw, Graphics::get(), std::cref(mesh->model)));
    }

    for (const auto& task : completed_tasks) {
        task.wait();
    }
}

void RenderSystem::PostUpdate() { Graphics::get()->Present(); }
