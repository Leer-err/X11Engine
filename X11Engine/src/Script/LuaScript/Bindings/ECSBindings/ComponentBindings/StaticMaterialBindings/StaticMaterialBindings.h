#pragma once

struct lua_State;

namespace Engine::Script::Binding::ECS {

void initStaticMaterialBindings(lua_State* state);

};