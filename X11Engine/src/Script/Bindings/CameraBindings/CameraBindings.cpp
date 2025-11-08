#include "CameraBindings.h"

#include "CameraManager.h"
#include "Engine.h"
#include "EntityBindings.h"
#include "EntityId.h"
#include "LuaUtility.h"

extern "C" int setCameraEntity(lua_State* state) {
    EntityId id = Engine::Script::Binding::ECS::getEntityId(state);

    World& world = Engine::Engine::get().getWorld();
    auto entity = world.getEntity(id);
    if (!entity) return 0;

    CameraManager::get().setMainCameraEntity(entity.value());

    return 0;
}

namespace Engine::Script::Binding::Camera {

void initBindings(lua_State* state) {
    lua_createtable(state, 0, 1);
    Utility::setMemberFunction(state, "setCameraEntity", setCameraEntity);
    Utility::setGlobalName(state, "CameraManager");
}

};  // namespace Engine::Script::Binding::Camera