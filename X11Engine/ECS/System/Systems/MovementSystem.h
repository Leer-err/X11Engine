#pragma once
#include "System.h"
#include "ECS/Component/Components/TransformComponent.h"
#include "Event/Event.h"
#include "Event/EventManager.h"

class MovementSystem : public ECS::System<MovementSystem> {
public:
	MovementSystem(TransformComponent* player) : m_player(player), m_moveX(0.f), m_moveZ(0.f) {}

	void PreUpdate() override {};
	void Update() override {
		m_player->position += vector3(m_moveX, 0.f, m_moveZ);
	};
	void PostUpdate() override { 
		m_moveX = 0.f;
		m_moveZ = 0.f;
	};

	void Move(const Event* event) {
		switch (((KeyDown*)event)->keyCode)
		{
		case 'W':
			m_moveZ += 1.f;
			break;
		case 'S':
			m_moveZ -= 1.f;
			break;
		case 'A':
			m_moveX -= 1.f;
			break;
		case 'D':
			m_moveX += 1.f;
			break;
		default:
			break;
		}
	}
private:
	TransformComponent* m_player;
	float m_moveX, m_moveZ;
};