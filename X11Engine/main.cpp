#include "Window.h"
#include <thread>
#include "Graphics/Graphics.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "SystemManager.h"
#include "Systems/RenderSystem.h"
#include "Systems/MovementSystem.h"
#include "Components/RenderComponent.h"
#include "Components/TransformComponent.h"
#include "Components/CameraComponent.h"
#include "Entities/Cube.h"
#include "TaskManager/TaskManager.h"
#include "Loader/Loader.h"
#include "Event/EventManager.h"
#include "Controls/Keyboard.h"

#include "Logger/Logger.h"

using std::thread;

void Update() {
	while (Window::get().IsRunning()) {
		EventManager::get()->DispatchEvents();
		ECS::SystemManager::Get()->PreUpdate();
		ECS::SystemManager::Get()->Update();
		ECS::SystemManager::Get()->PostUpdate();
	}
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	Window::get();
	Model m = Loader::get().LoadModelFromFile("Minecraft_Axolotl.fbx");

	EntityId a = ECS::EntityManager::Get()->CreateEntity<Cube>();
	EntityId camera = ECS::EntityManager::Get()->CreateEntity<Cube>();
	TransformComponent* b = ECS::ComponentManager::Get()->AddComponent<TransformComponent>(a, vector3( -0.5f, 0.0f, 3.f ));
	TransformComponent* cameraPos = ECS::ComponentManager::Get()->AddComponent<TransformComponent>(camera, vector3( 0.0f, 0.0f, 0.0f ));
	CameraComponent* cameraComponent = ECS::ComponentManager::Get()->AddComponent<CameraComponent>(camera, vector3(0.f, 0.f, 1.f), cameraPos);

	ECS::ComponentManager::Get()->AddComponent<RenderComponent>(a, m);

	MovementSystem* s = ECS::SystemManager::Get()->AddSystem<MovementSystem>(cameraPos);
	ECS::SystemManager::Get()->AddSystem<RenderSystem>();

	EventDelegate down = {&Keyboard::OnKeyDown, Keyboard::get()};
	EventDelegate up = {&Keyboard::OnKeyUp, Keyboard::get()};
	EventManager::get()->AddEventCallback(EventType::KeyDown, &down);
	EventManager::get()->AddEventCallback(EventType::KeyUp, &up);

	thread th2(Update);
	Window::get().Run();
	th2.join();
	return 0;
}
