#include "StaticMaterialBindings.h"

#include <cstddef>
#include <lua.hpp>

#include "ComponentMetatables.h"
#include "ComponentPool.h"
#include "LuaUtility.h"
#include "Scene.h"
#include "StaticMaterial.h"
#include "Texture.h"
#include "TextureBindings.h"
#include "World.h"

namespace Utility = Engine::Script::Utility;
namespace StaticModel = Asset::StaticModel;

extern "C" int setAlbedoTexture(lua_State* state) {
    World& world = Scene::get().getWorld();

    ComponentId* id = Utility::getUserData<ComponentId>(state, 1);
    auto component = world.get<StaticModel::Material>(*id);
    if (component == nullptr) return 0;

    auto texture = Engine::Script::Binding::getTexture(state, 2);
    component->albedo = texture;

    return 0;
}

namespace Engine::Script::Binding::ECS {

static void createStaticMaterialMetatable(lua_State* state) {
    lua_createtable(state, 0, 3);

    lua_pushvalue(state, -1);
    Utility::setMemberName(state, "__index");
    Utility::setMemberVariable(state, "__name", STATIC_MATERIAL_METATABLE);
    Utility::setMemberVariable(state, "setAlbedoTexture", setAlbedoTexture);
    Utility::setGlobalName(state, STATIC_MATERIAL_METATABLE);
}

void initStaticMaterialBindings(lua_State* state) {
    createStaticMaterialMetatable(state);
}

};  // namespace Engine::Script::Binding::ECS