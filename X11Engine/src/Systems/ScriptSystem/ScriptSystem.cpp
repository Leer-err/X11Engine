#include "ScriptSystem.h"

#include <tracy/Tracy.hpp>
#include <vector>

// #include "Bindings.h"
#include "Entity.h"
// #include "ScriptLoader.h"
#include "Scripts.h"
#include "World.h"

void ScriptSystem::update(World& world, float delta_time) {
    ZoneScoped;

    std::vector<Entity> entities = world.query().with<Scripts>().execute();

    for (auto& entity : entities) {
        auto scripts = entity.get<Scripts>();

        for (auto& script : scripts->scripts)
            script->update(entity, delta_time);
    }

    // sandbox.runFunction("Update", delta_time);
}