#include "TransformSystem.h"

#include <tracy/Tracy.hpp>
#include <vector>

#include "Children.h"
#include "Entity.h"
#include "Matrix.h"
#include "ObserverDispatcher.h"
#include "Parent.h"
#include "PhysicsComponents.h"
#include "PositionComponents.h"
#include "Quaternion.h"
#include "StaticRigidBody.h"
#include "Transform.h"
#include "World.h"

TransformSystem::TransformSystem(World& world) {
    auto transform_ditry_marker = [](Entity entity) {
        entity.add<DirtyTransform>();
    };

    auto child_dirty_marker = [](Entity entity) {
        if (entity.has<Children>() == false) return;

        Children children = *entity.get<Children>();

        for (auto& child : children.children) {
            child.add<DirtyTransform>();
        }
    };

    world.observer<Transform>().on(Event::Set).call(transform_ditry_marker);
    world.observer<DirtyTransform>().on(Event::Add).call(child_dirty_marker);
}

bool TransformSystem::prepare() { return true; }

void TransformSystem::update(World& world, float delta_time) {
    ZoneScoped;

    std::vector<Entity> top_level_entities = world.query()
                                                 .with<Transform>()
                                                 .with<DirtyTransform>()
                                                 .without<Parent>()
                                                 .execute();

    for (auto& entity : top_level_entities) {
        const Transform* transform = entity.get<Transform>();

        Matrix matrix = transform->toMatrix();
        entity.set<LocalMatrix>({matrix});
        entity.set<GlobalMatrix>({matrix});

        entity.remove<DirtyTransform>();
    }

    while (true) {
        std::vector<Entity> entities = world.query()
                                           .with<Transform>()
                                           .with<DirtyTransform>()
                                           .with<Parent>()
                                           .execute();

        if (entities.size() == 0) break;

        for (auto& entity : entities) {
            const Parent* parent = entity.get<Parent>();

            if (parent->parent.has<DirtyTransform>() == true) return;

            entity.remove<DirtyTransform>();

            const GlobalMatrix* parent_matrix_component =
                parent->parent.get<GlobalMatrix>();
            const Transform* transform = entity.get<Transform>();

            Matrix parent_matrix = parent_matrix_component->matrix;
            Matrix matrix = transform->toMatrix();
            Matrix global_matrix = parent_matrix * matrix;
            entity.set<LocalMatrix>({matrix});
            entity.set<GlobalMatrix>({global_matrix});

            const StaticRigidBodyComponent* static_rigid =
                entity.get<StaticRigidBodyComponent>();
            if (static_rigid != nullptr) {
                Vector3 global_position = global_matrix.getTranslation();
                Quaternion global_rotation = global_matrix.getRotation();

                static_rigid->body->setTransform(global_position,
                                                 global_rotation);
            }

            const DynamicRigidBodyComponent* dynamic_rigid =
                entity.get<DynamicRigidBodyComponent>();
            if (dynamic_rigid != nullptr) {
                Vector3 global_position = global_matrix.getTranslation();
                Quaternion global_rotation = global_matrix.getRotation();

                const Velocity* velocity = entity.get<Velocity>();

                dynamic_rigid->body->setVelocity(velocity->velocity);
                dynamic_rigid->body->setTransform(global_position,
                                                  global_rotation);
            }
        }
    }
}