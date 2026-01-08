#include "Scene.h"

void Scene::update(float deltaTime) {
    sky.draw();
    world.update(deltaTime);
}