#pragma once

#include "Sky.h"
#include "World.h"

class Scene {
   public:
    static Scene& get() {
        static Scene instance;
        return instance;
    }

    void update(float deltaTime);

    World& getWorld();

   private:
    Scene();

    void setupSystems();

    Sky sky;

    World world;
    Entity player;
};