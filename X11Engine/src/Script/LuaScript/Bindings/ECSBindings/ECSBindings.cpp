#include "ECSBindings.h"

#include <optional>

#include "ComponentMetatables.h"
#include "Engine.h"
#include "Entity.h"
#include "EntityBindings.h"
#include "EntityId.h"
#include "LuaUtility.h"
#include "PlayerComponent.h"
#include "TransformBindings.h"
#include "lua.h"
#include "lua.hpp"

namespace Utility = Engine::Script::Utility;

extern "C" int createEntity(lua_State* state) {
    World& world = Scene::get().getWorld();
    auto entity = world.createEntity();

    lua_createtable(state, 0, 1);
    Utility::setMemberVariable(state, "id", entity.getId());
    Utility::setMetatable(state, "Entity");

    return 1;
}

extern "C" int destroyEntity(lua_State* state) {
    World& world = Scene::get().getWorld();

    Utility::getMemberByName(state, "id", 1);

    EntityId entity_id = Utility::getArgument<long long>(state);
    auto entity = world.getEntity(entity_id);
    if (entity == std::nullopt) return 0;
    world.killEntity(entity.value());

    return 0;
}

void createWorldTable(lua_State* state) {
    lua_createtable(state, 0, 2);

    Utility::setMemberFunction(state, "createEntity", createEntity);
    Utility::setMemberFunction(state, "destroyEntity", destroyEntity);
    Utility::setGlobalName(state, "World");
}

void Engine::Script::Binding::ECS::initBindings(lua_State* state) {
    createWorldTable(state);

    initEntityBindings(state);
    createComponentTable(state);
    initComponentBindings(state);
}