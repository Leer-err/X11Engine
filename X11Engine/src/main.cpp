#include <Windows.h>

#include <cstdint>
#include <memory>
#include <thread>

#include "Dx11Factory.h"
#include "Engine/Engine.h"
#include "Logger/Logger.h"
#include "Logger/LoggerFactory.h"
#include "Window.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
            int nCmdShow) {
    Logger main_logger = LoggerFactory::getLogger("Main");
    main_logger.info("Starting");

    constexpr uint32_t INITIAL_WIDTH = 800;
    constexpr uint32_t INITIAL_HEIGHT = 600;

    auto window = std::make_shared<Window>();
    window->init(INITIAL_WIDTH, INITIAL_HEIGHT);
    main_logger.info("Created window with size {}x{}", window->getWidth(),
                     window->getHeight());

    auto factory = std::make_shared<Dx11Factory>(
        window->getHandle(), window->getWidth(), window->getHeight());
    main_logger.info("Created DirectX 11 resource factory", window->getWidth(),
                     window->getHeight());

    Engine::Engine engine;
    engine.init(factory, window);

    main_logger.info("Starting main loop");
    std::thread engine_thread = std::thread(&Engine::Engine::run, &engine);

    while (window->processMessages()) {
    }

    main_logger.info("Exiting");
    engine.exit();

    engine_thread.join();

    //  Logger::get()->Debug("Initializing");

    // ECS::SystemManager::get()->AddSystem<MovementSystem>();
    // ECS::SystemManager::get()->AddSystem<LookSystem>();
    // ECS::SystemManager::get()->AddSystem<RenderSystem>();
    // ECS::SystemManager::get()->AddSystem<AnimationSystem>();

    // Loader::get()->LoadScene("scene.json");

    // VertexShader vs = Graphics::get()->CreateVertexShader(
    //     Loader::get()->CompileVertexShaderFromFile(
    //         L"Shaders\\SkyboxVShader.hlsl", "main", shaderFlags));
    // PixelShader ps = Graphics::get()->CreatePixelShader(
    //     Loader::get()->CompilePixelShaderFromFile(
    //         L"Shaders\\SkyboxPShader.hlsl", "main", shaderFlags));
    // Graphics::get()->SetSkyboxShaders(ps, vs);

    // Graphics::get()->SetSkyboxMesh();

    // Interface::init(Graphics::get()->GetDevice(),
    // Graphics::get()->GetContext(),
    //                 Window::get()->GetHandle());

    // Timer::get()->Update();
    // thread th2(Update);
    // Window::get()->Run();
    // th2.join();
    return 0;
}
