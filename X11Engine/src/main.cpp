#include <Windows.h>

#include <cstdint>
#include <memory>
#include <thread>

#include "Config/Window/WindowConfig.h"
#include "Dx11Factory.h"
#include "Engine/Engine.h"
#include "Logger/Logger.h"
#include "Logger/LoggerFactory.h"
#include "Window.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
            int nCmdShow) {
    Logger main_logger = LoggerFactory::getLogger("Main");
    main_logger.info("Starting");

    WindowConfig config = {800, 600};
    Window& window = Window::get();
    window.init(config);

    auto factory = std::make_shared<Dx11Factory>(
        window.getHandle(), window.getWidth(), window.getHeight());
    main_logger.info("Created DirectX 11 resource factory");

    Engine::Engine::get().init(factory);

    main_logger.info("Starting main loop");
    std::thread engine_thread =
        std::thread(&Engine::Engine::run, &Engine::Engine::get());

    while (Window::get().processMessages()) {
    }

    main_logger.info("Exiting");
    Engine::Engine::get().exit();

    engine_thread.join();
    return 0;
}
