#pragma once
#include "ECS/System/System.h"
#include "Controls/Keyboard.h"

class MovementSystem : public ECS::System<MovementSystem> {
public:
	MovementSystem(TransformComponent* player) : m_player(player) {}

	void PreUpdate() override {};
	void Update() override {
		vector3 move = { 0.f, 0.f, 0.f };

		if (Keyboard::get()->IsPressed('W')) {
			move.z += 0.1f;
		}
		if (Keyboard::get()->IsPressed('S')) {
			move.z -= 0.1f;
		}
		if (Keyboard::get()->IsPressed('A')) {
			move.x -= 0.1f;
		}
		if (Keyboard::get()->IsPressed('D')) {
			move.x += 0.1f;
		}
		if (Keyboard::get()->IsPressed('E')) {
			move.y += 0.1f;
		}
		if (Keyboard::get()->IsPressed('Q')) {
			move.y -= 0.1f;
		}

		m_player->position += move.rotate(m_player->rotation);
	};
	void PostUpdate() override {};
private:
	TransformComponent* m_player;
};