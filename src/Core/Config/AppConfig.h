#pragma once

#include <string>

#include "GraphicsConfig.h"
#include "WindowConfig.h"

namespace Config {

struct AppConfig {
    std::string name;
    Graphics graphics;
    Window window;
};

class App {
   public:
    static App& get() {
        static App instance;
        return instance;
    }

    Graphics getGraphicsConfig() const;
    Window getWindowConfig() const;
    std::string getName() const;

   private:
    App();

    App(const App&) = delete;
    App& operator=(const App&) = delete;
    App(App&&) = delete;
    App& operator=(App&&) = delete;

    void loadDefaults();

    std::string name;

    Graphics graphics;
    Window window;
};

};  // namespace Config
