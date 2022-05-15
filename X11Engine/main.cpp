#include "Window.h"
#include <thread>
#include <atomic>
#include "Graphics/Graphics.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "Components/MeshComponent.h"
#include "Components/PositionComponent.h"
#include "Components/CameraComponent.h"
#include "Entities/Cube.h"
#include "TaskManager/TaskManager.h"

using std::thread;

std::atomic<bool> IsRunning = true;

void Update(Graphics& g, CameraComponent* camera) {
	while (IsRunning) {
		vector<future<void>> completed_tasks;
		g.PreFrame(camera);
		for (auto mesh = ECS::ComponentManager::Get()->begin<MeshComponent>(); mesh != ECS::ComponentManager::Get()->end<MeshComponent>(); ++mesh) {
			g.Draw(mesh->vertices, mesh->indices);
			completed_tasks.emplace_back(TaskManager::get().submit(&Graphics::Draw, &g, std::ref(mesh->vertices), std::ref(mesh->indices)));
		}
		for (const auto& task : completed_tasks) {
			task.wait();
		}
		g.PostFrame();
	}
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	TaskManager::get();

	EntityId a = ECS::EntityManager::Get()->CreateEntity<Cube>();
	EntityId camera = ECS::EntityManager::Get()->CreateEntity<Cube>();
	PositionComponent* b = ECS::ComponentManager::Get()->AddComponent<PositionComponent>(a, vector3( 0.0f, 0.5f, 0.0f ));
	PositionComponent* cameraPos = ECS::ComponentManager::Get()->AddComponent<PositionComponent>(camera, vector3( 0.0f, 0.0f, 0.0f ));
	CameraComponent* cameraComponent = ECS::ComponentManager::Get()->AddComponent<CameraComponent>(camera, cameraPos);

	vector<vector3> vert = {
		{0.5f, 0.5f, 0.5f},
		{-0.5f, 0.5f, 0.5f},
		{0.5f, -0.5f, 0.5f},
		{-0.5f, -0.5f, 0.5f}
	};
	vector<uint32_t> ind = {
		0, 2, 1,
		1, 2, 3
	};

	ECS::ComponentManager::Get()->AddComponent<MeshComponent>(a, vert, ind);

	Window::Get();
	//thread th(&Window::Run, &Window::Get());
	Graphics g{ 1920, 1080, Window::Get().GetHandle() };
	thread th2(Update, std::ref(g), cameraComponent);
	Window::Get().Run();
	IsRunning = false;
	th2.join();
	//th.join();
	return 0;
}
