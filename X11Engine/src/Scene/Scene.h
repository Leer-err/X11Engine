#pragma once

#include "Sky.h"
#include "World.h"

class Scene {
   public:
    static Scene get() {
        static Scene instance;
        return instance;
    }

    void update(float deltaTime);

   private:
    Scene() = default;

    Sky sky;

    // World world;
};