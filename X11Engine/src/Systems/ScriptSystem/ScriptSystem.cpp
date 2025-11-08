#include "ScriptSystem.h"

#include <tracy/Tracy.hpp>

#include "Bindings.h"
#include "ScriptLoader.h"

using namespace Engine::Script;

bool ScriptSystem::prepare(World& world) {
    sandbox.initBindings(world);

    ScriptLoader(sandbox).loadFromDirectory(
        "E:\\repos\\X11Engine\\X11Engine\\Scripts");

    sandbox.runFunction("Init");

    return true;
}

void ScriptSystem::update(World& world, float delta_time) {
    ZoneScoped;

    sandbox.runFunction("Update", delta_time);
}