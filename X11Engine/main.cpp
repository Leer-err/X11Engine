#include <thread>

#include "Engine/ECS/Component/ComponentManager.h"
#include "Engine/ECS/Component/Components/CameraComponent.h"
#include "Engine/ECS/Component/Components/PointLightComponent.h"
#include "Engine/ECS/Component/Components/RenderComponent.h"
#include "Engine/ECS/Component/Components/TransformComponent.h"
#include "Engine/ECS/Entity/EntityManager.h"
#include "Engine/ECS/System/SystemManager.h"
#include "Engine/ECS/System/Systems/AnimationSystem.h"
#include "Engine/ECS/System/Systems/LookSystem.h"
#include "Engine/ECS/System/Systems/MovementSystem.h"
#include "Engine/ECS/System/Systems/RenderSystem.h"
#include "Engine/Event/EventManager.h"
#include "Framework/Graphics/Graphics.h"
#include "Framework/Logger/Logger.h"
#include "Framework/TaskManager/TaskManager.h"
#include "Framework/Timer/Timer.h"
#include "Framework/Window/Interface/Interface.h"
#include "Framework/Window/Window.h"
#include "Loader/Loader.h"

using std::thread;

void Update() {
    while (Window::get()->IsRunning()) {
        Timer::get()->Update();
        auto deltaTime = Timer::get()->GetDeltaTime();

        EventManager::get()->DispatchEvents();
        ECS::SystemManager::get()->PreUpdate();
        ECS::SystemManager::get()->Update(deltaTime);
        Interface::get()->Update();
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

    Interface::init(Graphics::get()->GetDevice(), Graphics::get()->GetContext(),
                    Window::get()->GetHandle());

    Timer::get()->Update();
    thread th2(Update);
    Window::get()->Run();
    th2.join();
    return 0;
}
