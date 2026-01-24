#include "TransformSystem.h"

#include <tracy/Tracy.hpp>
#include <vector>

#include "Children.h"
#include "Entity.h"
#include "Matrix.h"
#include "Parent.h"
#include "Transform.h"
#include "World.h"
#include "WorldMatrix.h"

void TransformSystem::preUpdate(World& world) {
    ZoneScoped;
    updateTransforms(world);
}

void TransformSystem::preRender(World& world) {
    ZoneScoped;
    updateTransforms(world);
}

void TransformSystem::updateTransforms(World& world) {
    auto dirty_filter = [](Entity entity) {
        return entity.get<Transform>()->isDirty();
    };

    auto dirty_entities =
        world.query().with<Transform>().filter(dirty_filter).execute();
    for (auto& entity : dirty_entities) markChildrenDirty(entity);

    dirty_entities =
        world.query().with<Transform>().filter(dirty_filter).execute();

    bool has_dirty = true;
    while (has_dirty) {
        has_dirty = false;

        for (auto& entity : dirty_entities) {
            updateSingleTransform(entity);
            has_dirty |= entity.get<Transform>()->isDirty();
        }
    }
}

void TransformSystem::updateSingleTransform(Entity entity) {
    auto transform = entity.get<Transform>();
    auto position = Matrix::translation(transform->getPosition());
    auto orientation = Matrix::rotation(transform->getOrientation());
    auto scale = Matrix::scale(transform->getScale());
    auto world_matrix = scale * orientation * position;

    auto parent = entity.get<Parent>();
    if (parent == nullptr) {
        transform->markClean();
        entity.set<WorldMatrix>({world_matrix});
        return;
    }

    auto parent_transform = parent->parent.get<Transform>();
    if (parent_transform->isDirty()) return;

    auto parent_world = parent->parent.get<WorldMatrix>();
    auto parent_matrix = parent_world->matrix;

    world_matrix = world_matrix * parent_matrix;
    entity.set<WorldMatrix>({world_matrix});
    transform->markClean();
}

void TransformSystem::markChildrenDirty(Entity parent) {
    auto children = parent.get<Children>();
    if (children == nullptr) return;

    for (auto child : children->children) {
        child.get<Transform>()->markDirty();
        markChildrenDirty(child);
    }
}
