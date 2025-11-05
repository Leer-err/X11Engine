#include "ECSBindings.h"

#include "Entity.h"
#include "EntityId.h"
#include "LuaUtility.h"
#include "lua.h"
#include "lua.hpp"

namespace Utility = Engine::Script::Utility;

extern "C" int createEntity(lua_State* state) {
    auto world =
        static_cast<World*>(lua_touserdata(state, lua_upvalueindex(1)));

    if (world == nullptr) return 0;

    auto entity = world->createEntity();

    lua_newtable(state);
    Utility::setMemberVariable(state, "id", entity.getId());

    return 1;
}

extern "C" int destroyEntity(lua_State* state) {
    auto world =
        static_cast<World*>(lua_touserdata(state, lua_upvalueindex(1)));

    Utility::getMemberByName(state, "id", 2);
    EntityId entity_id = Utility::getArgument<long long>(state);

    auto entity = world->getEntity(entity_id);
    world->killEntity(entity);

    return 0;
}

extern "C" int entityAddComponent(lua_State* state) {
    auto world =
        static_cast<World*>(lua_touserdata(state, lua_upvalueindex(1)));
    auto entity_id = lua_tointeger(state, 2);

    if (world == nullptr) {
        Utility::pushNil(state);
        return 1;
    }

    auto entity = world->getEntity(entity_id);

    if (entity == std::nullopt) {
        Utility::pushNil(state);
        return 1;
    }

    Utility::pushArgument(state, entity->getId());

    return 1;
}

void createEntityMetatable(Lua_state* state, const World& world,
                           const std::string& name) {
    lua_newtable(state);

    lua_pushvalue(state, -1);
    Utility::setMemberName(state, "__index");
    Utility::setMemberFunction(state, "addComponent", entityAddComponent, -1,
                               &world);
    Utility::setMemberFunction(state, "hasComponent", entityHasComponent, -1,
                               &world);
    Utility::setMemberFunction(state, "removeComponent", entityRemoveComponent,
                               -1, &world);
    Utility::setGlobalName(state, "Entity");
}

void Engine::Script::Binding::ECS::initECSBindings(ScriptSandbox& sandbox,
                                                   World& world) {
    createEntityMetatable(sandbox.getState())

        sandbox.createTable("World")
            .addMethod("createEntity", createEntity, &world)
            .addMethod("destroyEntity", destroyEntity, &world);
}
