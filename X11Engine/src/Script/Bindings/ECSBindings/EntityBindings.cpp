#include "EntityBindings.h"

#include <tracy/Tracy.hpp>

#include "ComponentPool.h"
#include "EntityId.h"
#include "LuaUtility.h"
#include "PlayerComponent.h"
#include "Transform.h"
#include "TransformBindings.h"
#include "TypeId.h"
#include "TypeIdHelper.h"
#include "World.h"

namespace Utility = Engine::Script::Utility;
namespace ECS = Engine::Script::Binding::ECS;

EntityId getEntityId(lua_State* state, int stack_index = 1) {
    Utility::getMemberByName(state, "id", 1);
    return Utility::getArgument<EntityId>(state);
}

extern "C" int entityAddComponent(lua_State* state) {
    World* world =
        static_cast<World*>(lua_touserdata(state, lua_upvalueindex(1)));

    auto entity = world->getEntity(getEntityId(state));
    if (!entity) return 0;

    TypeId type = Utility::getArgument<TypeId>(state, 2);

    if (type == TypeIdHelper::getTypeId<Transform>()) entity->add<Transform>();
    if (type == TypeIdHelper::getTypeId<Player>()) entity->add<Player>();

    return 0;
}

extern "C" int entityRemoveComponent(lua_State* state) {
    World* world =
        static_cast<World*>(lua_touserdata(state, lua_upvalueindex(1)));

    auto entity = world->getEntity(getEntityId(state));
    if (!entity) return 0;

    TypeId type = Utility::getArgument<TypeId>(state, 2);

    if (type == TypeIdHelper::getTypeId<Transform>())
        entity->remove<Transform>();

    return 0;
}

extern "C" int entityHasComponent(lua_State* state) {
    World* world =
        static_cast<World*>(lua_touserdata(state, lua_upvalueindex(1)));

    auto entity = world->getEntity(getEntityId(state));
    if (!entity) {
        Utility::pushNil(state);
        return 0;
    }

    TypeId type = Utility::getArgument<TypeId>(state, 2);

    bool result = false;
    if (type == TypeIdHelper::getTypeId<Transform>())
        result = entity->has<Transform>();

    Utility::pushArgument(state, result);

    return 1;
}

extern "C" int entityGetTransform(lua_State* state) {
    ZoneScoped;
    World* world =
        static_cast<World*>(lua_touserdata(state, lua_upvalueindex(1)));

    Utility::getMemberByName(state, "id", 1);
    EntityId entity_id = Utility::getArgument<long long>(state);

    auto entity = world->getEntity(entity_id);
    if (!entity) {
        Utility::pushNil(state);
        return 0;
    }

    ComponentId component = entity->getComponentId<Transform>();
    ECS::pushTransform(state, component);

    return 1;
}

void createEntityTable(lua_State* state, World* world) {
    lua_createtable(state, 0, 5);

    lua_pushvalue(state, -1);
    Utility::setMemberName(state, "__index");
    Utility::setMemberFunction(state, "addComponent", entityAddComponent, -1,
                               world);
    Utility::setMemberFunction(state, "hasComponent", entityHasComponent, -1,
                               world);
    Utility::setMemberFunction(state, "removeComponent", entityRemoveComponent,
                               -1, world);
    Utility::setMemberFunction(state, "getTransform", entityGetTransform, -1,
                               world);
    Utility::setGlobalName(state, "Entity");
}

void createComponentTable(lua_State* state, World* world) {
    lua_createtable(state, 0, 1);

    Utility::setMemberVariable(state, "Transform",
                               TypeIdHelper::getTypeId<Transform>());
    Utility::setMemberVariable(state, "Player",
                               TypeIdHelper::getTypeId<Player>());
    Utility::setGlobalName(state, "Components");
}

void Engine::Script::Binding::ECS::initEntityBindings(lua_State* state,
                                                      World* world) {
    createEntityTable(state, world);
    createComponentTable(state, world);
    initTransformBindings(state, world);
}
