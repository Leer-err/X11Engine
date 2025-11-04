#include "PhysicsSystem.h"

#include <memory>
#include <tracy/Tracy.hpp>
#include <vector>

#include "DynamicRigidBody.h"
#include "Entity.h"
#include "Matrix.h"
#include "Parent.h"
#include "PhysicsComponents.h"
#include "PhysicsFactory.h"
#include "PositionComponents.h"
#include "Quaternion.h"
#include "Transform.h"
#include "Vector3.h"
#include "World.h"

struct DirtyVelocity {};

PhysicsSystem::PhysicsSystem(std::shared_ptr<PhysicsFactory> factory,
                             World& world, float step)
    : factory(factory), accumulated_time(0) {
    fixed_step = step;

    auto velocity_ditry_marker = [](Entity entity) {
        entity.add<DirtyVelocity>();
    };

    world.observer<Velocity>().on(Event::Set).call(velocity_ditry_marker);
}

PhysicsSystem::~PhysicsSystem() {}

bool PhysicsSystem::prepare(World& world) { return true; }

void PhysicsSystem::update(World& world, float delta_time) {
    ZoneScoped;

    accumulated_time += delta_time;

    if (accumulated_time < fixed_step) return;

    accumulated_time -= fixed_step;

    updateDirtyDynamics(world);
    updateDirtyStatics(world);

    factory->simulate(fixed_step);

    std::vector<Entity> dynamic_rigids =
        world.query().with<DynamicRigidBodyComponent>().execute();

    for (auto& dynamic_rigid_body : dynamic_rigids) {
        const Parent* parent = dynamic_rigid_body.get<Parent>();

        Matrix parent_matrix = Matrix::identity();

        if (parent != nullptr) {
            const GlobalMatrix* parent_matrix_component =
                parent->parent.get<GlobalMatrix>();

            parent_matrix = parent_matrix_component->matrix;
        }

        Vector3 global_position =
            dynamic_rigid_body.get<DynamicRigidBodyComponent>()
                ->body->getPosition();
        Quaternion global_rotation =
            dynamic_rigid_body.get<DynamicRigidBodyComponent>()
                ->body->getRotation();

        Matrix global_matrix = Matrix::rotation(global_rotation) *
                               Matrix::translation(global_position);

        Matrix local_matrix = global_matrix * parent_matrix.inverse();

        Transform tr;
        tr.position = local_matrix.getTranslation();
        tr.orientation = local_matrix.getRotation();
        dynamic_rigid_body.set<Transform>(tr);

        Vector3 velocity = dynamic_rigid_body.get<DynamicRigidBodyComponent>()
                               ->body->getVelocity();
        dynamic_rigid_body.set<Velocity>({velocity});
        dynamic_rigid_body.remove<DirtyVelocity>();
    }
}

void PhysicsSystem::updateDirtyDynamics(World& world) {
    std::vector<Entity> entities = world.query()
                                       .with<DynamicRigidBodyComponent>()
                                       .with<DirtyPhysics>()
                                       .execute();

    for (auto& entity : entities) {
        auto body = entity.get<DynamicRigidBodyComponent>()->body;
        Matrix global_matrix = entity.get<GlobalMatrix>()->matrix;

        Vector3 global_position = global_matrix.getTranslation();
        Quaternion global_rotation = global_matrix.getRotation();

        const Velocity* velocity = entity.get<Velocity>();
        if (velocity != nullptr && entity.has<DirtyVelocity>()) {
            body->setVelocity(velocity->velocity);
            entity.remove<DirtyVelocity>();
        }
        body->setTransform(global_position, global_rotation);
        entity.remove<DirtyPhysics>();
    }

    std::vector<Entity> velocity_entities =
        world.query()
            .with<DynamicRigidBodyComponent>()
            .with<DirtyVelocity>()
            .execute();

    for (auto& entity : velocity_entities) {
        auto body = entity.get<DynamicRigidBodyComponent>()->body;

        const Velocity* velocity = entity.get<Velocity>();
        body->setVelocity(velocity->velocity);
        entity.remove<DirtyVelocity>();
    }
}

void PhysicsSystem::updateDirtyStatics(World& world) {
    std::vector<Entity> entities = world.query()
                                       .with<StaticRigidBodyComponent>()
                                       .with<DirtyPhysics>()
                                       .execute();

    for (auto& entity : entities) {
        auto body = entity.get<StaticRigidBodyComponent>()->body;
        Matrix global_matrix = entity.get<GlobalMatrix>()->matrix;

        Vector3 global_position = global_matrix.getTranslation();
        Quaternion global_rotation = global_matrix.getRotation();

        body->setTransform(global_position, global_rotation);
        entity.remove<DirtyPhysics>();
    }
}