#include "EntityBindings.h"

#include <optional>
#include <tracy/Tracy.hpp>

#include "ComponentMetatables.h"
#include "ComponentPool.h"
#include "EntityId.h"
#include "LuaUtility.h"
#include "Scene.h"
#include "TypeId.h"

namespace Utility = Engine::Script::Utility;
namespace ECS = Engine::Script::Binding::ECS;

std::optional<Entity> getEntity(lua_State* state, int stack_index = 1) {
    World& world = Scene::get().getWorld();

    return world.getEntity(ECS::getEntityId(state, stack_index));
}

extern "C" int entityAddComponent(lua_State* state) {
    auto entity = getEntity(state);
    if (!entity) return 0;

    TypeId type = Utility::getArgument<TypeId>(state, 2);

    ECS::addComponent(state, *entity, type);

    return 0;
}

extern "C" int entityRemoveComponent(lua_State* state) {
    auto entity = getEntity(state);
    if (!entity) return 0;

    TypeId type = Utility::getArgument<TypeId>(state, 2);

    entity->remove(type);

    return 0;
}

extern "C" int entityHasComponent(lua_State* state) {
    auto entity = getEntity(state);
    if (!entity) return Utility::pushNil(state);

    TypeId type = Utility::getArgument<TypeId>(state, 2);

    auto result = entity->has(type);
    Utility::pushArgument(state, result);
    return 1;
}

extern "C" int entityGetComponent(lua_State* state) {
    using namespace Engine::Script::Binding::ECS;

    ZoneScoped;
    auto entity = getEntity(state);
    if (!entity) return Utility::pushNil(state);

    TypeId type = Utility::getArgument<TypeId>(state, 2);

    return ECS::getComponent(state, *entity, type);
}

extern "C" int entityAddChild(lua_State* state) {
    ZoneScoped;
    auto entity = getEntity(state);
    auto child = getEntity(state, 2);

    if (!entity || !child) return 0;

    entity->addChild(*child);

    return 0;
}

void createEntityTable(lua_State* state) {
    lua_createtable(state, 0, 5);

    lua_pushvalue(state, -1);
    Utility::setMemberName(state, "__index");
    Utility::setMemberFunction(state, "addComponent", entityAddComponent);
    Utility::setMemberFunction(state, "hasComponent", entityHasComponent);
    Utility::setMemberFunction(state, "getComponent", entityGetComponent);
    Utility::setMemberFunction(state, "removeComponent", entityRemoveComponent);
    Utility::setMemberFunction(state, "addChild", entityAddChild);
    Utility::setGlobalName(state, "Entity");
}

namespace Engine::Script::Binding::ECS {

EntityId getEntityId(lua_State* state, int stack_index) {
    Utility::getMemberByName(state, "id", stack_index);
    return Utility::getArgument<EntityId>(state);
}

void initEntityBindings(lua_State* state) { createEntityTable(state); }

};  // namespace Engine::Script::Binding::ECS
