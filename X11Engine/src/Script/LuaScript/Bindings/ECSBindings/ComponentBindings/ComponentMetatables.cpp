#include "ComponentMetatables.h"

#include <lua.hpp>

#include "Entity.h"
#include "LuaUtility.h"
#include "MeshBindings/MeshBindings.h"
#include "StaticMaterial.h"
#include "StaticMesh.h"
#include "Transform.h"
#include "TransformBindings.h"
#include "TypeIdHelper.h"
#include "lua.h"

namespace Utility = Engine::Script::Utility;
namespace StaticModel = Asset::StaticModel;

namespace Engine::Script::Binding::ECS {

void addComponent(lua_State* state, Entity entity, TypeId component_type) {
    if (component_type == TypeIdHelper::getTypeId<Transform>())
        entity.add<Transform>();
    else if (component_type == TypeIdHelper::getTypeId<StaticMesh>())
        entity.add<StaticMesh>();
    else if (component_type == TypeIdHelper::getTypeId<StaticModel::Material>())
        entity.add<StaticMesh>();
}

int getComponent(lua_State* state, Entity entity, TypeId component_type) {
    const char* metatable_name;
    if (component_type == TypeIdHelper::getTypeId<Transform>())
        metatable_name = TRANSFORM_METATABLE;
    else if (component_type == TypeIdHelper::getTypeId<StaticMesh>())
        metatable_name = STATIC_MESH_METATABLE;
    else
        return Utility::pushNil(state);

    ComponentId component = entity.getComponentId(component_type);
    Utility::pushUserData(state, metatable_name, component);

    return 1;
}

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