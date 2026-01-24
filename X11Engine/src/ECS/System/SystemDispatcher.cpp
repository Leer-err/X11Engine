#include "SystemDispatcher.h"

#include <tracy/Tracy.hpp>


void SystemDispatcher::progress(World& world, float delta_time) {
    ZoneScoped;

    preSimulate(world);
    simulate(world);
    preUpdate(world);
    update(world, delta_time);
    preRender(world);
    render(world);
}

void SystemDispatcher::preSimulate(World& world) {
    ZoneScoped;
    for (const auto& system : systems) system->preSimulate(world);
}

void SystemDispatcher::simulate(World& world) {
    ZoneScoped;
    for (const auto& system : systems) system->simulate(world);
}

void SystemDispatcher::preUpdate(World& world) {
    ZoneScoped;
    for (const auto& system : systems) system->preUpdate(world);
}

void SystemDispatcher::update(World& world, float delta_time) {
    ZoneScoped;
    for (const auto& system : systems) system->update(world, delta_time);
}

void SystemDispatcher::preRender(World& world) {
    ZoneScoped;
    for (const auto& system : systems) system->preRender(world);
}

void SystemDispatcher::render(World& world) {
    ZoneScoped;
    for (const auto& system : systems) system->render(world);
}
