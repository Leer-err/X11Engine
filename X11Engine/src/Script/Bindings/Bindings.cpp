#include "Bindings.h"

#include "ECSBindings.h"
#include "InputBindings.h"
#include "World.h"

using namespace Engine::Script;

namespace Engine::Script::Binding {

void initBindings(ScriptSandbox& sandbox, World& world) {
    Input::initInputBindings(sandbox);
    ECS::initECSBindings(sandbox, world);
}

};  // namespace Engine::Script::Binding