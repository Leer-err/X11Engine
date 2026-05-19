#pragma once

#include "Camera/Camera.h"
#include "IScene.h"
#include "Sky.h"
#include "World.h"

class Scene : public IScene {
   public:
    Scene();

    void update(float deltaTime) override;

   private:
    void setupSystems();

    Sky sky;

    Camera camera;

    World world;
};