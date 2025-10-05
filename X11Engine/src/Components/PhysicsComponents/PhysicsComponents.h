#ifndef PHYSICS_COMPONENTS_H
#define PHYSICS_COMPONENTS_H

#include <memory>
#include <string>
#include <vector>

#include "DynamicRigidBody.h"
#include "Entity.h"
#include "StaticRigidBody.h"

struct StaticRigidBodyComponent {
    std::shared_ptr<StaticRigidBody> body;

    std::string serialize() const;
};

struct DynamicRigidBodyComponent {
    std::shared_ptr<DynamicRigidBody> body;

    std::string serialize() const;
};

struct Velocity {
    Vector3 velocity;

    std::string serialize() const;
};

struct Collisions {
    std::vector<Entity> collision_list;
};

struct DirtyPhysics {};

#endif  // PHYSICS_COMPONENTS_H