#include "PreSimulateUpdateSystem.h"

#include <deque>
#include <tracy/Tracy.hpp>
#include <vector>

#include "Children.h"
#include "Entity.h"
#include "Matrix.h"
#include "ObserverDispatcher.h"
#include "Parent.h"
#include "PhysicsComponents.h"
#include "PositionComponents.h"
#include "Transform.h"
#include "World.h"

bool PreSimulateUpdateSystem::prepare(World& world) { return true; }

void PreSimulateUpdateSystem::update(World& world, float delta_time) {
    ZoneScoped;

    std::vector<Entity> top_level_entities = world.query()
                                                 .with<Transform>()
                                                 .with<DirtyTransform>()
                                                 .without<Parent>()
                                                 .execute();

    std::deque<Entity> entities(top_level_entities.begin(),
                                top_level_entities.end());

    while (entities.empty() == false) {
        Entity entity = entities.front();
        entities.pop_front();

        const Children* children = entity.get<Children>();
        if (children != nullptr) {
            for (auto& child : children->children) {
                entities.push_back(child);
            }
        }

        Matrix parent_matrix = Matrix::identity();

        const Parent* parent = entity.get<Parent>();
        if (parent != nullptr) {
            const GlobalMatrix* parent_matrix_component =
                parent->parent.get<GlobalMatrix>();
            parent_matrix = parent_matrix_component->matrix;
        }

        const Transform* transform = entity.get<Transform>();

        Matrix matrix = transform->toMatrix();
        Matrix global_matrix = parent_matrix * matrix;
        entity.set<LocalMatrix>({matrix});
        entity.set<GlobalMatrix>({global_matrix});

        entity.remove<DirtyTransform>();

        if (entity.has<DynamicRigidBodyComponent>() ||
            entity.has<StaticRigidBodyComponent>()) {
            entity.add<DirtyPhysics>();
        }
    }
}