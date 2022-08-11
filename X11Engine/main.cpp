#include <thread>

#include "ComponentManager.h"
#include "Components/CameraComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/RenderComponent.h"
#include "Components/TransformComponent.h"
#include "Controls/Keyboard.h"
#include "Controls/Mouse.h"
#include "Entities/Cube.h"
#include "EntityManager.h"
#include "Event/EventManager.h"
#include "Graphics/Graphics.h"
#include "Loader/Loader.h"
#include "Logger/Logger.h"
#include "SystemManager.h"
#include "Systems/LookSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/RenderSystem.h"
#include "TaskManager/TaskManager.h"
#include "Window.h"


using std::thread;

void Update() {
    while (Window::get()->IsRunning()) {
        EventManager::get()->DispatchEvents();
        ECS::SystemManager::get()->PreUpdate();
        ECS::SystemManager::get()->Update();
        ECS::SystemManager::get()->PostUpdate();
    }
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
            int nCmdShow) {
    Window::get();
    Model* m =
        Loader::get()->LoadModelFromFile("\\assets\\Minecraft_Axolotl.fbx");

    EntityId a = ECS::EntityManager::get()->CreateEntity<Cube>();
    EntityId light = ECS::EntityManager::get()->CreateEntity<Cube>();
    EntityId light1 = ECS::EntityManager::get()->CreateEntity<Cube>();
    EntityId camera = ECS::EntityManager::get()->CreateEntity<Cube>();
    ECS::ComponentManager::get()->AddComponent<TransformComponent>(
        a, vector3(5.f, 0.0f, 0.f), vector3(0.f, 0.f, 0.f));
    ECS::ComponentManager::get()->AddComponent<TransformComponent>(
        light, vector3(0.f, 0.f, 0.f));
    ECS::ComponentManager::get()->AddComponent<TransformComponent>(
        light1, vector3(0.f, 0.f, 2.f));
    TransformComponent* cameraPos =
        ECS::ComponentManager::get()->AddComponent<TransformComponent>(
            camera, vector3(0.0f, 0.0f, 0.0f));
    ECS::ComponentManager::get()->AddComponent<CameraComponent>(
        camera, vector3(0.f, 0.f, 1.f));

    ECS::ComponentManager::get()->AddComponent<RenderComponent>(a, m);

    ECS::ComponentManager::get()->AddComponent<PointLightComponent>(
        light, PointLight{1.f,
                          .09f,
                          0.032f,
                          {.0f, .0f, .05f},
                          {1.f, 1.f, 1.f},
                          {1.f, 1.f, 1.f}});
    ECS::ComponentManager::get()->AddComponent<PointLightComponent>(
        light1, PointLight{1.f,
                           .0003f,
                           0.00007f,
                           {.0f, .0f, .05f},
                           {1.f, 1.f, 1.f},
                           {1.f, 1.f, 1.f}});

    ECS::SystemManager::get()->AddSystem<MovementSystem>(cameraPos);
    ECS::SystemManager::get()->AddSystem<LookSystem>(cameraPos);
    ECS::SystemManager::get()->AddSystem<RenderSystem>();

    EventManager::get()->AddEventCallback(
        EventType::KeyDown, &Keyboard::OnKeyDown, Keyboard::get());
    EventManager::get()->AddEventCallback(EventType::KeyUp, &Keyboard::OnKeyUp,
                                          Keyboard::get());
    EventManager::get()->AddEventCallback(EventType::MouseMove, &Mouse::OnMove,
                                          Mouse::get());

    Graphics::get()->SetDirLight({{0.f, 0.f, 1.f},
                                  {.05f, .05f, .05f},
                                  {1.f, 1.f, 1.f},
                                  {1.f, 1.f, 1.f}});

    thread th2(Update);
    Window::get()->Run();
    th2.join();
    return 0;
}
