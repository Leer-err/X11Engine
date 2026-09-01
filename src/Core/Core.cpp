#include "Core.h"

#include "EssentialsLoader.h"
#include "Graphics.h"
#include "LoggerFactory.h"
#include "Window.h"

namespace Core {

bool init() {
    auto logger = LoggerFactory::getLogger("Core");

    Window::get().init();
    if (Graphics::init() == false) {
        logger.error("Failed to initialize graphics module");
        return false;
    }

    if (Essentials::load() == false) {
        logger.error("Failed to initialize essential resources");
        return false;
    }

    logger.info("Core initialized successfully");
    return true;
}

}  // namespace Core