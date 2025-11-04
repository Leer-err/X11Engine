#include "Bindings.h"

#include "ECSBindings.h"
#include "InputBindings.h"
#include "World.h"

using namespace Engine::Script;

namespace Engine::Script::Binding {

void initBindings(lua_State* state, World& world) {
    Input::initInputBindings(state);
    ECS::initECSBindings(state, world);
}

};  // namespace Engine::Script::Binding