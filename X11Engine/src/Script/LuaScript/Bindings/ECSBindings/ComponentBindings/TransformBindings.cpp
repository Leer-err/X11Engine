#include "TransformBindings.h"

#include <tracy/Tracy.hpp>

#include "ComponentPool.h"
#include "Engine.h"
#include "LuaUtility.h"
#include "Transform.h"
#include "Vector3.h"
#include "Vector3Bindings.h"
#include "World.h"

namespace Utility = Engine::Script::Utility;

extern "C" int getPosition(lua_State* state) {
    ZoneScoped;
    World& world = Scene::get().getWorld();

    ComponentId* id = Utility::getUserData<ComponentId>(state, 1);
    auto transform = world.get<Transform>(*id);
    if (transform == nullptr) {
        Utility::pushNil(state);
        return 1;
    }

    Engine::Script::Binding::Types::pushVector3(state,
                                                transform->getPosition());

    return 1;
}

extern "C" int setPosition(lua_State* state) {
    ZoneScoped;
    World& world = Scene::get().getWorld();

    ComponentId* id = Utility::getUserData<ComponentId>(state, 1);
    auto transform = world.get<Transform>(*id);
    if (transform == nullptr) return 0;

    Vector3 position = Engine::Script::Binding::Types::toVector3(state, 2);
    transform->setPosition(position);

    return 0;
}

extern "C" int getForward(lua_State* state) {
    ZoneScoped;
    World& world = Scene::get().getWorld();

    ComponentId* id = Utility::getUserData<ComponentId>(state, 1);
    auto transform = world.get<Transform>(*id);
    if (transform == nullptr) {
        Utility::pushNil(state);
        return 1;
    }

    Vector3 forward = transform->getForward();
    Engine::Script::Binding::Types::pushVector3(state, forward);

    return 1;
}

extern "C" int getRight(lua_State* state) {
    ZoneScoped;
    World& world = Scene::get().getWorld();

    ComponentId* id = Utility::getUserData<ComponentId>(state, 1);
    auto transform = world.get<Transform>(*id);
    if (transform == nullptr) {
        Utility::pushNil(state);
        return 1;
    }

    Vector3 right = transform->getRight();
    Engine::Script::Binding::Types::pushVector3(state, right);

    return 1;
}

extern "C" int getUp(lua_State* state) {
    ZoneScoped;
    World& world = Scene::get().getWorld();

    ComponentId* id = Utility::getUserData<ComponentId>(state, 1);
    auto transform = world.get<Transform>(*id);
    if (transform == nullptr) {
        Utility::pushNil(state);
        return 1;
    }

    Vector3 up = transform->getUp();
    Engine::Script::Binding::Types::pushVector3(state, up);

    return 1;
}

namespace Engine::Script::Binding::ECS {

void pushTransform(lua_State* state, ComponentId transform) {
    Utility::pushUserData(state, "Transform", transform);
}

void createTransformMetatable(lua_State* state) {
    lua_createtable(state, 0, 5);

    lua_pushvalue(state, -1);
    Utility::setMemberName(state, "__index");
    Utility::setMemberVariable(state, "__name", "TransformComponent");
    Utility::setMemberFunction(state, "getPosition", getPosition);
    Utility::setMemberFunction(state, "setPosition", setPosition);
    Utility::setMemberFunction(state, "getForward", getForward);
    Utility::setMemberFunction(state, "getRight", getRight);
    Utility::setMemberFunction(state, "getUp", getUp);
    Utility::setGlobalName(state, "Transform");
}

void initTransformBindings(lua_State* state) {
    createTransformMetatable(state);
}

};  // namespace Engine::Script::Binding::ECS