#include "PlayerAttackSystem.h"

#include "Entity.h"
#include "Input.h"
#include "PlayerComponent.h"
#include "Transform.h"
#include "World.h"

bool PlayerAttackSystem::prepare() { return true; }

void PlayerAttackSystem::update(World& world, float delta_time) {
    std::vector<Entity> entities =
        world.query().with<Player>().with<Transform>().execute();

    for (auto& player : entities) {
        Transform transform = *player.get<Transform>();

        InputState state = Input::get().getState();

        Vector3 movement = {};

        if (state.actionStates[(int)Action::ATTACK] == ActionState::ON)
            movement.z += 1;

        movement = movement * delta_time;

        transform.position = transform.position + movement;
        player.set(transform);
    }
}