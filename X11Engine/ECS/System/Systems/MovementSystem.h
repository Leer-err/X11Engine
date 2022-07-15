#pragma once
#include "System.h"
#include "Controls/Keyboard.h"

class MovementSystem : public ECS::System<MovementSystem> {
public:
	MovementSystem(TransformComponent* player) : m_player(player) {}

	void PreUpdate() override {};
	void Update() override {
		vector3 move = { 0.f, 0.f, 0.f };

		if (Keyboard::get()->IsPressed('W')) {
			move.z += 1.f;
		}
		if (Keyboard::get()->IsPressed('S')) {
			move.z -= 1.f;
		}
		if (Keyboard::get()->IsPressed('A')) {
			move.x -= 1.f;
		}
		if (Keyboard::get()->IsPressed('D')) {
			move.x += 1.f;
		}
		if (Keyboard::get()->IsPressed('E')) {
			move.y += 1.f;
		}
		if (Keyboard::get()->IsPressed('Q')) {
			move.y -= 1.f;
		}

		m_player->position += move;
	};
	void PostUpdate() override {};
private:
	TransformComponent* m_player;
};