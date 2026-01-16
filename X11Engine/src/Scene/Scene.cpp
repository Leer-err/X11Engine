#include "Scene.h"

#include <memory>

#include "CameraManager.h"
#include "Name.h"
#include "PlayerLook.h"
#include "ScriptSystem.h"
#include "StaticProjectionCamera.h"
#include "Transform.h"
#include "TransformSystem.h"

Scene::Scene() {
    auto cam = std::make_shared<StaticProjectionCamera>(60, (float)4 / 3, 0.1f,
                                                        1000.f);
    CameraManager::get().setMainCamera(cam);

    setupSystems();

    player = world.createEntity();
    player.set<Name>({"Player"});
    player.set<Transform>({});
    player.addScript(std::make_unique<Scripts::Player::PlayerLookX>());

    Entity camera = world.createEntity();
    camera.set<Name>({"Camera"});
    camera.set<Transform>({});
    camera.addScript(std::make_unique<Scripts::Player::CameraLookY>());
    CameraManager::get().setMainCameraEntity(camera);

    player.addChild(camera);
}

void Scene::update(float deltaTime) {
    sky.draw();
    world.update(deltaTime);

    CameraManager::get().updateCameraData();
}

void Scene::setupSystems() {
    world.add<ScriptSystem>();
    world.add<TransformSystem>();
}
