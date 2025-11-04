#include "EntityRegistry.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <vector>

#include "Entity.h"
#include "EntityId.h"

TEST(EntityRegistryTest, CreateEntityTest) {
    EntityRegistry reg;

    reg.createEntity(nullptr);

    std::vector<Entity> entities = reg.getEntities();

    ASSERT_EQ(entities.size(), 1);
    ASSERT_EQ(entities[0].getId(), 1);
}

TEST(EntityRegistryTest, KillEntityTest) {
    EntityRegistry reg;

    Entity first_entity = reg.createEntity(nullptr);
    Entity second_entity = reg.createEntity(nullptr);

    EntityId first_id = first_entity.getId();
    EntityId second_id = second_entity.getId();

    reg.killEntity(first_entity);

    std::vector<Entity> entities = reg.getEntities();

    ASSERT_EQ(entities.size(), 1);
    ASSERT_EQ(entities[0].getId(), second_id);
}

TEST(EntityRegistryTest, GetEntityTest) {
    EntityRegistry reg;

    Entity entity = reg.createEntity(nullptr);

    Entity copy = reg.getEntityFromId(entity.getId()).value();

    ASSERT_EQ(entity, copy);
}