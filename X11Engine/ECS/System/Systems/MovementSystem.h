#pragma once

// #include "Controls/Keyboard.h"
#include <Keyboard.h>

#include "ECS/Component/Components/TransformComponent.h"
#include "ECS/System/System.h"
#include "Timer/Timer.h"

class MovementSystem : public ECS::System<MovementSystem> {
   public:
    MovementSystem() : m_player(nullptr) {}

    void PreUpdate() override{};
    void Update(float delta) override {
        assert(m_player != nullptr);

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

        move = move.normalized() * 5.f * delta;

        m_player->SetPosition(m_player->GetPosition() +
                              move.rotate(m_player->GetRotation()));
    };
    void PostUpdate() override{};

    void SetPlayer(TransformComponent* playerTransform) {
        m_player = playerTransform;
    }

   private:
    TransformComponent* m_player;
};