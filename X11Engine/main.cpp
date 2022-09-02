#include <thread>

#include "ComponentManager.h"
#include "Components/CameraComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/RenderComponent.h"
#include "Components/TransformComponent.h"
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
#include "Timer/Timer.h"
#include "Window.h"

using std::thread;

void Update() {
    while (Window::get()->IsRunning()) {
        Timer::get()->Update();
        EventManager::get()->DispatchEvents();
        ECS::SystemManager::get()->PreUpdate();
        ECS::SystemManager::get()->Update();
        ECS::SystemManager::get()->PostUpdate();
    }
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
            int nCmdShow) {
    Window::get();

    Loader::get()->LoadScene("scene.json");

    VertexShader vs = Graphics::get()->CreateVertexShader(
        Loader::get()->CompileVertexShaderFromFile(
            L"Shaders\\SkyboxVShader.hlsl", "main", shaderFlags));
    PixelShader ps = Graphics::get()->CreatePixelShader(
        Loader::get()->CompilePixelShaderFromFile(
            L"Shaders\\SkyboxPShader.hlsl", "main", shaderFlags));
    Graphics::get()->SetSkyboxShaders(ps, vs);

    Graphics::get()->SetSkyboxMesh();

    Model* m =
        Loader::get()->LoadModelFromFile("\\assets\\Minecraft_Axolotl.fbx");

    EntityId model = ECS::EntityManager::get()->CreateEntity<Cube>();
    EntityId camera = ECS::EntityManager::get()->CreateEntity<Cube>();

    TransformComponent* modelPos = model.AddComponent<TransformComponent>(
        Scene::get()->GetWorldNode(), vector3(0.f, 0.f, 3.f),
        vector3(0.f, 0.f, 0.f));
    TransformComponent* cameraPos = camera.AddComponent<TransformComponent>(
        Scene::get()->GetWorldNode(), vector3(0.0f, 0.0f, 0.0f));
    camera.AddComponent<CameraComponent>(
        1000.f, .01f, DirectX::XMConvertToRadians(60.f), 16.f / 9.f);

    model.AddComponent<RenderComponent>(modelPos->sceneNode, m);

    ECS::SystemManager::get()->AddSystem<MovementSystem>(cameraPos);
    ECS::SystemManager::get()->AddSystem<LookSystem>(cameraPos);
    ECS::SystemManager::get()->AddSystem<RenderSystem>()->SetCamera(camera);

    thread th2(Update);
    Window::get()->Run();
    th2.join();
    return 0;
}
