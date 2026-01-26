#include "MeshBindings.h"

#include <lua.hpp>

#include "ComponentMetatables.h"
#include "LuaUtility.h"
#include "MeshRegistry.h"
#include "Scene.h"
#include "StaticMesh.h"
#include "World.h"

namespace Utility = Engine::Script::Utility;

extern "C" int setMesh(lua_State* state) {
    World& world = Scene::get().getWorld();

    ComponentId* id = Utility::getUserData<ComponentId>(state, 1);
    auto component = world.get<StaticMesh>(*id);
    if (component == nullptr) return Utility::pushNil(state);

    std::string filename = Utility::getArgument<std::string>(state, 2);
    auto mesh = MeshRegistry().get(filename);

    component->mesh = mesh;

    return 0;
}

namespace Engine::Script::Binding::ECS {

void createMeshMetatable(lua_State* state) {
    lua_createtable(state, 0, 3);

    lua_pushvalue(state, -1);
    Utility::setMemberName(state, "__index");
    Utility::setMemberVariable(state, "__name", STATIC_MESH_METATABLE);
    Utility::setMemberVariable(state, "setMesh", setMesh);
    Utility::setGlobalName(state, STATIC_MESH_METATABLE);
}

void initMeshBindings(lua_State* state) { createMeshMetatable(state); }

};  // namespace Engine::Script::Binding::ECS