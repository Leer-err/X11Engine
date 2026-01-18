#include "ComponentMetatables.h"

#include <lua.hpp>

#include "LuaUtility.h"
#include "MeshBindings/MeshBindings.h"
#include "TransformBindings.h"
#include "TypeIdHelper.h"
#include "lua.h"

struct Transform;
struct StaticMesh;

namespace Utility = Engine::Script::Utility;

namespace Engine::Script::Binding::ECS {
void createComponentTable(lua_State* state) {
    lua_createtable(state, 0, 1);

    Utility::setMemberVariable(state, "Transform",
                               TypeIdHelper::getTypeId<Transform>());
    Utility::setMemberVariable(state, "StaticMesh",
                               TypeIdHelper::getTypeId<StaticMesh>());
    Utility::setGlobalName(state, "Components");
}

void initComponentBindings(lua_State* state) {
    initTransformBindings(state);
    initMeshBindings(state);
}

};  // namespace Engine::Script::Binding::ECS