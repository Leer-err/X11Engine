#include <thread>

#include "ComponentManager.h"
#include "Components/CameraComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/RenderComponent.h"
#include "Components/TransformComponent.h"
#include "ECS/Entity/EntityManager.h"
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

    ECS::SystemManager::get()->AddSystem<MovementSystem>();
    ECS::SystemManager::get()->AddSystem<LookSystem>();
    ECS::SystemManager::get()->AddSystem<RenderSystem>();

    Loader::get()->LoadScene("scene.json");

    VertexShader vs = Graphics::get()->CreateVertexShader(
        Loader::get()->CompileVertexShaderFromFile(
            L"Shaders\\SkyboxVShader.hlsl", "main", shaderFlags));
    PixelShader ps = Graphics::get()->CreatePixelShader(
        Loader::get()->CompilePixelShaderFromFile(
            L"Shaders\\SkyboxPShader.hlsl", "main", shaderFlags));
    Graphics::get()->SetSkyboxShaders(ps, vs);

    Graphics::get()->SetSkyboxMesh();

    thread th2(Update);
    Window::get()->Run();
    th2.join();
    return 0;
}
