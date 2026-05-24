#pragma once

#include <memory>

#include "Entity.h"
#include "GameInputContext.h"
#include "IScript.h"
#include "Vector3.h"

class MoveScript final : public IScript {
   public:
    MoveScript(const Entity& player,
               const std::shared_ptr<Input::GameInputContext>& input);

    void update(float delta_time) override;

   private:
    Vector3 position;

    float speed;
    Entity player;

    std::shared_ptr<Input::GameInputContext> input;
};