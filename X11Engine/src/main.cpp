#include <Windows.h>

#include "Engine.h"
#include "Logger.h"
#include "LoggerFactory.h"
#include "Window.h"
#include "WindowConfig.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
            int nCmdShow) {
    Logger main_logger = LoggerFactory::getLogger("Main");
    main_logger.info("Starting");

    WindowConfig config = {};
    config.width = 1800;
    config.height = 1000;

    Window::get().init(config);

    Engine::Engine::get().init();

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
