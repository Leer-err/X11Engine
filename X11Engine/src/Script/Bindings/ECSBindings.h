#include "ScriptSandbox.h"
#include "World.h"

namespace Engine::Script::Binding::ECS {

void initECSBindings(lua_State* state, World& world);

};  // namespace Engine::Script::Binding::ECS