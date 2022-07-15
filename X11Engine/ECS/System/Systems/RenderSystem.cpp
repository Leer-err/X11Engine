#include "RenderSystem.h"

#include <vector>
#include <future>

#include "Graphics/Graphics.h"
#include "ECS/Component/ComponentManager.h"
#include "TaskManager/TaskManager.h"
#include "ECS/Component/Components/RenderComponent.h"
#include "ECS/Component/Components/TransformComponent.h"
#include "ECS/Component/Components/CameraComponent.h"

using std::future;
using std::vector;

void RenderSystem::PreUpdate()
{
	Graphics::get().Clear();
}

void RenderSystem::Update()
{
	vector<future<void>> completed_tasks;
	CameraComponent* camera = ECS::ComponentManager::Get()->begin<CameraComponent>().Get();
	matrix viewMatrix = LookToMatrix(camera->position->position, camera->viewDirection, { 0.f, 1.f, 0.f });
	for (auto mesh = ECS::ComponentManager::Get()->begin<RenderComponent>(); mesh != ECS::ComponentManager::Get()->end<RenderComponent>(); ++mesh) {
		EntityId entity = mesh->GetOwner();
		const TransformComponent* pos = ECS::ComponentManager::Get()->GetComponent<TransformComponent>(entity);
		matrix model = ScalingMatrix(pos->scale) * RotationMatrix(pos->rotation) * TranslationMatrix(pos->position);
		matrix mvpMatrix = model * viewMatrix * camera->projectionMatrix;
		completed_tasks.emplace_back(TaskManager::get().submit(&Graphics::Draw, &Graphics::get(), std::ref(mesh->model), std::ref(mvpMatrix)));
	}
	for (const auto& task : completed_tasks) {
		task.wait();
	}
}

void RenderSystem::PostUpdate()
{
	Graphics::get().Present();
}
