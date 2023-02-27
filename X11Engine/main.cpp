#include <thread>

#include "ECS/Component/ComponentManager.h"
#include "ECS/Component/Components/CameraComponent.h"
#include "ECS/Component/Components/PointLightComponent.h"
#include "ECS/Component/Components/RenderComponent.h"
#include "ECS/Component/Components/TransformComponent.h"
#include "ECS/Entity/EntityManager.h"
#include "ECS/System/SystemManager.h"
#include "ECS/System/Systems/AnimationSystem.h"
#include "ECS/System/Systems/LookSystem.h"
#include "ECS/System/Systems/MovementSystem.h"
#include "ECS/System/Systems/RenderSystem.h"
#include "Event/EventManager.h"
#include "Graphics/Graphics.h"
#include "Loader/Loader.h"
#include "Logger/Logger.h"
#include "TaskManager/TaskManager.h"
#include "Timer/Timer.h"
#include "Window.h"

using std::thread;

void Update() {
    while (Window::get()->IsRunning()) {
        Timer::get()->Update();
        auto deltaTime = Timer::get()->GetDeltaTime();

        EventManager::get()->DispatchEvents();
        ECS::SystemManager::get()->PreUpdate();
        ECS::SystemManager::get()->Update(deltaTime);
        ECS::SystemManager::get()->PostUpdate();
    }
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
            int nCmdShow) {
    Window::get();

    ECS::SystemManager::get()->AddSystem<MovementSystem>();
    ECS::SystemManager::get()->AddSystem<LookSystem>();
    ECS::SystemManager::get()->AddSystem<RenderSystem>();
    ECS::SystemManager::get()->AddSystem<AnimationSystem>();

    Loader::get()->LoadScene("scene.json");

    VertexShader vs = Graphics::get()->CreateVertexShader(
        Loader::get()->CompileVertexShaderFromFile(
            L"Shaders\\SkyboxVShader.hlsl", "main", shaderFlags));
    PixelShader ps = Graphics::get()->CreatePixelShader(
        Loader::get()->CompilePixelShaderFromFile(
            L"Shaders\\SkyboxPShader.hlsl", "main", shaderFlags));
    Graphics::get()->SetSkyboxShaders(ps, vs);

    Graphics::get()->SetSkyboxMesh();

    Timer::get()->Update();
    thread th2(Update);
    Window::get()->Run();
    th2.join();
    return 0;
}
