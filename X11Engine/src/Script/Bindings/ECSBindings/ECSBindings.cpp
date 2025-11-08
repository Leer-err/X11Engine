#include "ECSBindings.h"

#include <optional>

#include "Entity.h"
#include "EntityBindings.h"
#include "EntityId.h"
#include "LuaUtility.h"
#include "PlayerComponent.h"
#include "TransformBindings.h"
#include "lua.h"
#include "lua.hpp"

namespace Utility = Engine::Script::Utility;

static World* getWorldFromStack(lua_State* state, int world_index) {
    return static_cast<World*>(lua_touserdata(state, lua_upvalueindex(1)));
}

extern "C" int createEntity(lua_State* state) {
    World* world = getWorldFromStack(state, 1);
    if (world == nullptr) return 0;

    auto entity = world->createEntity();

    lua_newtable(state);
    Utility::setMemberVariable(state, "id", entity.getId());
    Utility::setMetatable(state, "Entity");

    return 1;
}

extern "C" int destroyEntity(lua_State* state) {
    World* world = getWorldFromStack(state, 1);

    Utility::getMemberByName(state, "id", 1);

    EntityId entity_id = Utility::getArgument<long long>(state);
    auto entity = world->getEntity(entity_id);
    if (entity == std::nullopt) return 0;
    world->killEntity(entity.value());

    return 0;
}

extern "C" int getPlayer(lua_State* state) {
    World* world = getWorldFromStack(state, 1);

    Entity player = world->query().with<Player>().execute()[0];

    lua_newtable(state);
    Utility::setMemberVariable(state, "id", player.getId());
    Utility::setMetatable(state, "Entity");

    return 1;
}

void createWorldTable(lua_State* state, World* world) {
    lua_newtable(state);

    Utility::setMemberFunction(state, "createEntity", createEntity, -1, world);
    Utility::setMemberFunction(state, "destroyEntity", destroyEntity, -1,
                               world);
    Utility::setMemberFunction(state, "getPlayer", getPlayer, -1, world);
    Utility::setGlobalName(state, "World");
}

void Engine::Script::Binding::ECS::initECSBindings(lua_State* state,
                                                   World* world) {
    createWorldTable(state, world);

    initEntityBindings(state, world);
}