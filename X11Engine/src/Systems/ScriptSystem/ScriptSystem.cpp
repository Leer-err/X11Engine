#include "ScriptSystem.h"

#include "Bindings.h"
#include "ScriptLoader.h"

using namespace Engine::Script;

bool ScriptSystem::prepare(World& world) {
    ScriptLoader(sandbox).loadFromDirectory(
        "E:\\repos\\X11Engine\\X11Engine\\Scripts");
    Engine::Script::Binding::initBindings(sandbox, world);

    sandbox.runFunction("Init");

    return true;
}

void ScriptSystem::update(World& world, float delta_time) {
    sandbox.runFunction("Update", delta_time);
}