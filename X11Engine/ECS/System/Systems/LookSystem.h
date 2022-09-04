#pragma once
#include <Mouse.h>

#include "ECS/Component/Components/TransformComponent.h"
#include "ECS/System/System.h"

class LookSystem : public ECS::System<LookSystem> {
   public:
    LookSystem() : m_player(nullptr), m_yaw(0.f), m_pitch(0.f) {}

    void PreUpdate() override {
        DirectX::Mouse::State state = DirectX::Mouse::Get().GetState();

        if (state.positionMode == DirectX::Mouse::MODE_RELATIVE) {
            m_pitch = 0.01f * state.y + m_pitch;
            m_yaw = 0.01f * state.x + m_yaw;

            if (m_yaw > 3.14f) m_yaw -= 6.28f;
            if (m_yaw < -3.14f) m_yaw += 6.28f;

            if (m_pitch > 1.57f) m_pitch = 1.57f;
            if (m_pitch < -1.57f) m_pitch = -1.57f;
        }

        quaternion newRotation = {m_pitch, m_yaw, 0.f};

        m_player->SetRotation(newRotation);
    }
    void Update() override {}
    void PostUpdate() override {}

    void SetPlayer(TransformComponent* playerTransform) {
        m_player = playerTransform;
    }

   private:
    TransformComponent* m_player;
    float m_pitch;
    float m_yaw;
};