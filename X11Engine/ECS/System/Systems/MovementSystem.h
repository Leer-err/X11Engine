#pragma once

//#include "Controls/Keyboard.h"
#include <Keyboard.h>

#include "ECS/Component/Components/TransformComponent.h"
#include "ECS/System/System.h"
#include "Timer/Timer.h"

class MovementSystem : public ECS::System<MovementSystem> {
   public:
    MovementSystem(TransformComponent* player) : m_player(player) {}

    void PreUpdate() override{};
    void Update() override {
        vector3 move = {0.f, 0.f, 0.f};
        DirectX::Keyboard::State state = DirectX::Keyboard::Get().GetState();

        if (state.W) {
            move.z += 1.f;
        }
        if (state.S) {
            move.z -= 1.f;
        }
        if (state.A) {
            move.x -= 1.f;
        }
        if (state.D) {
            move.x += 1.f;
        }
        if (state.E) {
            move.y += 1.f;
        }
        if (state.Q) {
            move.y -= 1.f;
        }

        move = move.normalized() * 5.f * Timer::get()->GetDeltaTime();

        m_player->SetPosition(m_player->GetPosition() +
                              move.rotate(m_player->GetRotation()));
    };
    void PostUpdate() override{};

   private:
    TransformComponent* m_player;
};