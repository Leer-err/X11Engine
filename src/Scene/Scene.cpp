#include "Scene.h"

#include <stb_image.h>

#include <memory>
#include <tracy/Tracy.hpp>

#include "Camera.h"
#include "Entity.h"
#include "GameInputContext/GameInputContext.h"
#include "Graphics.h"
#include "GraphicsCommunicationManager.h"
#include "LookScript.h"
#include "ModelReader.h"
#include "PhysicalInput.h"
#include "Quaternion.h"
#include "ScriptSystem.h"
#include "Scripts/MoveScript.h"
#include "StaticModelData.h"
#include "Transform.h"
#include "TransformSystem.h"
#include "Vector3.h"

StaticModelData model_data = {};

Scene::Scene() {
    setupSystems();

    camera = Camera::create(60, 16.f / 9, 1, 1000);

    File::ModelReader reader("./Assets/gem2.fbx");
    auto vertices = reader.readVertices();
    auto indices = reader.readIndices();

    auto renderer = Graphics::getRenderEngine();
    model_data.position = {0, 0, 10};
    model_data.mesh =
        renderer->addMesh(vertices.data(), vertices.size() * sizeof(Vertex),
                          indices.data(), indices.size() * sizeof(uint32_t));

    int width;
    int height;
    int channels;
    unsigned char* data =
        stbi_load("./Assets/gem2.png", &width, &height, &channels, 0);

    model_data.albedo = renderer->addTexture(data, width, height);

    std::shared_ptr<Input::GameInputContext> input =
        std::make_shared<Input::GameInputContext>();
    input->addBinding(Input::GameAxes::LookYaw, Input::Axis::MOUSE_X);
    input->addBinding(Input::GameAxes::LookPitch, Input::Axis::MOUSE_Y);
    input->addBinding(Input::GameAxes::MoveForwardBackward,
                      Input::Button::KEYBOARD_W, 1);
    input->addBinding(Input::GameAxes::MoveForwardBackward,
                      Input::Button::KEYBOARD_S, -1);
    input->addBinding(Input::GameAxes::MoveLeftRight, Input::Button::KEYBOARD_D,
                      1);
    input->addBinding(Input::GameAxes::MoveLeftRight, Input::Button::KEYBOARD_A,
                      -1);
    input->addBinding(Input::GameAxes::MoveUpDown,
                      Input::Button::KEYBOARD_LSHIFT, 1);
    input->addBinding(Input::GameAxes::MoveUpDown,
                      Input::Button::KEYBOARD_LCTRL, -1);

    Entity player = world.createEntity();
    Entity head = world.createEntity();
    head.set<Transform>({});
    player.addChild(head);
    player.set<Transform>({});
    player.addScript(std::make_unique<LookScript>(head, player, camera, input));
    player.addScript(std::make_unique<MoveScript>(player, input));
}

void Scene::update(float deltaTime) {
    ZoneScoped;
    auto& graphics_communicator = GraphicsCommunicationManager::get();

    sky.draw();
    world.update(deltaTime);

    graphics_communicator.send(model_data);
}

void Scene::setupSystems() {
    world.addSystem<TransformSystem>();
    world.addSystem<ScriptSystem>();
}
