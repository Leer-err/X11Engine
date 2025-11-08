#include "EntityBindings.h"

#include <string>
#include <tracy/Tracy.hpp>

#include "Children.h"
#include "ComponentPool.h"
#include "Engine.h"
#include "EntityId.h"
#include "LuaUtility.h"
#include "Name.h"
#include "PlayerComponent.h"
#include "Transform.h"
#include "TransformBindings.h"
#include "TypeId.h"
#include "TypeIdHelper.h"
#include "World.h"

namespace Utility = Engine::Script::Utility;
namespace ECS = Engine::Script::Binding::ECS;

extern "C" int entityAddComponent(lua_State* state) {
    World& world = Engine::Engine::get().getWorld();

    auto entity = world.getEntity(ECS::getEntityId(state));
    if (!entity) return 0;

    TypeId type = Utility::getArgument<TypeId>(state, 2);

    if (type == TypeIdHelper::getTypeId<Transform>()) entity->add<Transform>();
    if (type == TypeIdHelper::getTypeId<Player>()) entity->add<Player>();

    return 0;
}

extern "C" int entityRemoveComponent(lua_State* state) {
    World& world = Engine::Engine::get().getWorld();

    auto entity = world.getEntity(ECS::getEntityId(state));
    if (!entity) return 0;

    TypeId type = Utility::getArgument<TypeId>(state, 2);

    if (type == TypeIdHelper::getTypeId<Transform>())
        entity->remove<Transform>();

    return 0;
}

extern "C" int entityHasComponent(lua_State* state) {
    World& world = Engine::Engine::get().getWorld();

    auto entity = world.getEntity(ECS::getEntityId(state));
    if (!entity) {
        Utility::pushNil(state);
        return 1;
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
    World& world = Engine::Engine::get().getWorld();

    Utility::getMemberByName(state, "id", 1);
    EntityId entity_id = Utility::getArgument<long long>(state);

    auto entity = world.getEntity(entity_id);
    if (!entity) {
        Utility::pushNil(state);
        return 1;
    }

    ComponentId component = entity->getComponentId<Transform>();
    ECS::pushTransform(state, component);

    return 1;
}

extern "C" int entityGetName(lua_State* state) {
    ZoneScoped;
    World& world = Engine::Engine::get().getWorld();

    Utility::getMemberByName(state, "id", 1);
    EntityId entity_id = Utility::getArgument<long long>(state);

    auto entity = world.getEntity(entity_id);
    if (!entity) {
        Utility::pushNil(state);
        return 1;
    }

    auto& name = entity->get<Name>()->name;
    Utility::pushArgument(state, name);

    return 1;
}

extern "C" int entitySetName(lua_State* state) {
    ZoneScoped;
    World& world = Engine::Engine::get().getWorld();

    Utility::getMemberByName(state, "id", 1);
    EntityId entity_id = Utility::getArgument<long long>(state);

    auto entity = world.getEntity(entity_id);
    if (!entity) {
        return 0;
    }

    auto name = Utility::getArgument<std::string>(state, 2);
    entity->set<Name>({name});

    return 0;
}

extern "C" int entityAddChild(lua_State* state) {
    ZoneScoped;
    World& world = Engine::Engine::get().getWorld();

    Utility::getMemberByName(state, "id", 1);
    EntityId entity_id = Utility::getArgument<long long>(state);

    auto entity = world.getEntity(entity_id);
    if (!entity) {
        return 0;
    }

    Utility::getMemberByName(state, "id", 2);
    EntityId child_id = Utility::getArgument<long long>(state);

    auto child = world.getEntity(child_id);
    if (!child) {
        return 0;
    }

    entity->addChild(*child);

    return 0;
}

void createEntityTable(lua_State* state) {
    lua_createtable(state, 0, 5);

    lua_pushvalue(state, -1);
    Utility::setMemberName(state, "__index");
    Utility::setMemberFunction(state, "addComponent", entityAddComponent);
    Utility::setMemberFunction(state, "hasComponent", entityHasComponent);
    Utility::setMemberFunction(state, "removeComponent", entityRemoveComponent);
    Utility::setMemberFunction(state, "getTransform", entityGetTransform);
    Utility::setMemberFunction(state, "setName", entitySetName);
    Utility::setMemberFunction(state, "getName", entityGetName);
    Utility::setMemberFunction(state, "addChild", entityAddChild);
    Utility::setGlobalName(state, "Entity");
}

void createComponentTable(lua_State* state) {
    lua_createtable(state, 0, 1);

    Utility::setMemberVariable(state, "Transform",
                               TypeIdHelper::getTypeId<Transform>());
    Utility::setGlobalName(state, "Components");
}

namespace Engine::Script::Binding::ECS {

EntityId getEntityId(lua_State* state, int stack_index) {
    Utility::getMemberByName(state, "id", stack_index);
    return Utility::getArgument<EntityId>(state);
}

void initEntityBindings(lua_State* state) {
    createEntityTable(state);
    createComponentTable(state);
    initTransformBindings(state);
}

};  // namespace Engine::Script::Binding::ECS
