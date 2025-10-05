#include "PlayerMovementSystem.h"

#include <tracy/Tracy.hpp>

#include "GameInputContext.h"
#include "Input.h"
#include "PlayerComponent.h"
#include "Quaternion.h"
#include "StaticProjectionCamera.h"
#include "Transform.h"
#include "Vector3.h"
#include "World.h"

bool PlayerMovementSystem::prepare() { return true; }

void PlayerMovementSystem::update(World& world, float delta_time) {
    ZoneScoped;

    std::vector<Entity> entities =
        world.query().with<Player>().with<Transform>().execute();

    for (auto& player : entities) {
        Transform transform = *player.get<Transform>();

        Vector3 movement = {};

        movement.z = GameInputContext::get().getAxis(MOVE_FORWARD_BACKWARD);
        movement.x = GameInputContext::get().getAxis(MOVE_LEFT_RIGHT);
        movement = movement.normalized();

        movement = movement * delta_time;

        transform.position = transform.position + movement;

        auto yaw = GameInputContext::get().getAxis(LOOK_X);

        auto rotation = Quaternion(0, yaw, 0);

        transform.orientation = transform.orientation * rotation;

        player.set(transform);

        std::vector<Entity> cameras = world.query()
                                          .with<StaticProjectionCamera>()
                                          .childOf(player)
                                          .execute();

        Transform camera_transform = *cameras[0].get<Transform>();

        auto pitch = GameInputContext::get().getAxis(LOOK_Y);

        auto camera_pitch = Quaternion(pitch, 0, 0);

        camera_transform.orientation =
            camera_transform.orientation * camera_pitch;

        cameras[0].set(camera_transform);
    }
}