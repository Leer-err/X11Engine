#include "Window.h"
#include <thread>
#include "Graphics/Graphics.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "Components/MeshComponent.h"
#include "Components/TransformComponent.h"
#include "Components/CameraComponent.h"
#include "Entities/Cube.h"
#include "TaskManager/TaskManager.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include "Loader/Loader.h"
#include "Graphics/Texture.h"

#include "Logger/Logger.h"

using std::thread;

void Update(CameraComponent* camera) {
	while (Window::get().IsRunning()) {
		vector<future<void>> completed_tasks;
		Graphics::get().PreFrame(camera);
		for (auto mesh = ECS::ComponentManager::Get()->begin<RenderComponent>(); mesh != ECS::ComponentManager::Get()->end<RenderComponent>(); ++mesh) {
			EntityId entity = mesh->GetOwner();
			const TransformComponent* pos = ECS::ComponentManager::Get()->GetComponent<TransformComponent>(entity);
			matrix model = ScalingMatrix(pos->scale) * RotationMatrix(pos->rotation) * TranslationMatrix(pos->position);
			matrix mvpMatrix = model * camera->viewMatrix * camera->projectionMatrix;
			completed_tasks.emplace_back(TaskManager::get().submit(&Graphics::Draw, &Graphics::get(), std::ref(mesh->model), std::ref(mvpMatrix)));
		}
		for (const auto& task : completed_tasks) {
			task.wait();
		}
		Graphics::get().PostFrame();
	}
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	Model m = Loader::get().LoadModelFromFile("Minecraft_Axolotl.fbx");

	EntityId a = ECS::EntityManager::Get()->CreateEntity<Cube>();
	EntityId camera = ECS::EntityManager::Get()->CreateEntity<Cube>();
	TransformComponent* b = ECS::ComponentManager::Get()->AddComponent<TransformComponent>(a, vector3( -0.5f, 0.0f, 3.f ), vector3(0.f, 0.f, 0.f));
	TransformComponent* cameraPos = ECS::ComponentManager::Get()->AddComponent<TransformComponent>(camera, vector3( 0.0f, 0.0f, 0.0f ));
	CameraComponent* cameraComponent = ECS::ComponentManager::Get()->AddComponent<CameraComponent>(camera, vector3(0.f, 0.f, 1.f), cameraPos);

	ECS::ComponentManager::Get()->AddComponent<RenderComponent>(a, m);

	Window::get();
	thread th2(Update, cameraComponent);
	Window::get().Run();
	th2.join();
	return 0;
}
