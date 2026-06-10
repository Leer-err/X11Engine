#pragma once

#include <memory>

#include "Camera.h"
#include "Entity.h"
#include "GameInputContext.h"
#include "IScript.h"
#include "RenderWorld/RenderWorld.h"

class LookScript final : public IScript {
   public:
    LookScript(const Entity& head_entity, const Entity& player_entity,
               const Camera& camera,
               const std::shared_ptr<Input::GameInputContext>& input);

    void update(float delta_time) override;
    void preRender() override;

   private:
    float current_pitch;
    float current_yaw;

    Camera camera;

    // This entity will be rotated on X axis
    Entity head_entity;
    // This entity will be rotated on Z axis
    Entity player_entity;

    std::shared_ptr<Input::GameInputContext> input;

    Graphics::RenderWorld& render_world;
};