#pragma once

#include <filesystem>
#include <optional>

#include "Camera.h"
#include "IScene.h"
#include "RenderObjectData.h"
#include "Sky.h"
#include "World.h"

class Scene : public IScene {
   public:
    Scene();

    void update(float deltaTime) override;

   private:
    static std::optional<RenderObjectData> readRenderObjectFile(
        const std::filesystem::path& path);

    void setupSystems();

    Sky sky;

    Camera camera;

    World world;
};