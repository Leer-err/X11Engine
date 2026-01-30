#pragma once

#include "GraphicsConfig.h"
#include "WindowConfig.h"

namespace Config {

class App {
   public:
    static App& get() {
        static App instance;
        return instance;
    }

    Graphics getGraphicsConfig();
    Window getWindowConfig();

   private:
    App();

    App(const App&) = delete;
    App& operator=(const App&) = delete;
    App(App&&) = delete;
    App& operator=(App&&) = delete;

    void loadDefaults();

    Graphics graphics;
    Window window;
};

};  // namespace Config
