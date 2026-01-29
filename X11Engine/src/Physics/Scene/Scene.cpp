#include "Scene.h"

namespace Physics {

Scene::Scene() {
    auto physics = Resources::getPhysics();

    auto dispatcher = physx::PxDefaultCpuDispatcherCreate(2);

    physx::PxSceneDesc desc(physics->getTolerancesScale());
    desc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    desc.cpuDispatcher = dispatcher;
    desc.filterShader = physx::PxDefaultSimulationFilterShader;
    desc.simulationEventCallback = &gCollisionCallback;  // Register callback
    scene = physics->createScene(sceneDesc);

    dispatcher.release();
}

void Scene::addActor(const Actor& actor) {}

void Scene::simulate(float delta_time) {
    scene->simulate(delta_time);
    scene->fetchResults(true);
}

};  // namespace Physics