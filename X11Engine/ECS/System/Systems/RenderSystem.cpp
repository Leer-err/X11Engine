#include "RenderSystem.h"

#include <future>
#include <stack>
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
using std::stack;
using std::vector;

void RenderSystem::PreUpdate() { Graphics::get()->Clear(); }

void RenderSystem::Update() {
    vector<future<void>> completed_tasks;

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

    Scene::get()->WaitForUpdate();
    Scene::Node* worldNode = Scene::get()->GetWorldNode();

    stack<Scene::Node*> nodes;
    nodes.push(worldNode);

    for (Scene::Node* node; nodes.size() != 0;) {
        node = nodes.top();
        nodes.pop();

        Model* model = node->GetModel();
        if (model != nullptr) {
            Graphics::get()->SetWorldMatrix(node->GetWorldMatrix());
            Graphics::get()->UpdatePerModelBuffers();
            completed_tasks.emplace_back(TaskManager::get()->submit(
                &Graphics::Draw, Graphics::get(), model));
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
