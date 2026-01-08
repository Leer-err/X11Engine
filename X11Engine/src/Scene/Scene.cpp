#include "Scene.h"

#include <memory>

#include "Name.h"
#include "PlayerLook.h"
#include "ScriptSystem.h"
#include "Transform.h"

Scene::Scene() {
    setupSystems();

    player = world.createEntity();
    player.set<Name>({"Player"});
    player.add<Transform>();
    player.addScript(std::make_unique<Scripts::Player::PlayerLookX>());

    Entity camera = world.createEntity();
    camera.set<Name>({"Camera"});
    camera.add<Transform>();
    camera.addScript(std::make_unique<Scripts::Player::CameraLookY>());

    player.addChild(camera);
}

void Scene::update(float deltaTime) {
    sky.draw();
    world.update(deltaTime);
}

void Scene::setupSystems() { world.addSystem<ScriptSystem>(); }
