#include "DeleteProjectileSystem.h"

#include <vector>

#include "AnimatedMesh.h"
#include "Entity.h"
#include "Model.h"
#include "PhysicsComponents.h"
#include "World.h"

bool DeleteProjectileSystem::prepare() { return true; }

void DeleteProjectileSystem::update(World& world, float delta_time) {
    std::vector<Entity> projectiles =
        world.query().with<DeleteProjectile>().with<Collisions>().execute();

    for (auto& projectile : projectiles) {
        const Collisions* collisions = projectile.get<Collisions>();

        if (collisions == nullptr) return;

        for (auto collision : collisions->collision_list) {
            world.killEntity(collision);
        }
    }
}