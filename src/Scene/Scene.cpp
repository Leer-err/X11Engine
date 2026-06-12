#include "Scene.h"

#include <stb_image.h>

#include <memory>
#include <tracy/Tracy.hpp>

#include "Camera.h"
#include "Entity.h"
#include "GameInputContext.h"
#include "Graphics.h"
#include "LookScript.h"
#include "ModelReader.h"
#include "MoveScript.h"
#include "OpaqueRenderObjectData.h"
#include "PhysicalInput.h"
#include "ScriptSystem.h"
#include "Transform.h"
#include "TransformSystem.h"

Scene::Scene() {
    setupSystems();

    camera = Camera::create(60, 16.f / 9, 1, 1000);

    auto renderer = Graphics::getRenderEngine();

    int width;
    int height;
    int channels;
    unsigned char* data;

    OpaqueRenderObjectData tower_data = {};
    auto tower_reader = File::ModelReader("./Assets/Tower.fbx");
    // auto vertices = tower_reader.readVertices();
    // auto indices = tower_reader.readIndices();
    // tower_data.position = {-26, 9, -8};
    // tower_data.mesh =
    //     renderer->addMesh(vertices.data(), vertices.size() * sizeof(Vertex),
    //                       indices.data(), indices.size() * sizeof(uint32_t));
    // data = stbi_load("./Assets/tower.png", &width, &height, &channels, 0);
    // tower_data.albedo = renderer->addTexture(data, width, height);
    // renderer->getRenderWorld().addOpaqueObject(tower_data);

    // OpaqueRenderObjectData gem_data = {};
    // auto gem_reader = File::ModelReader("./Assets/Gem2.fbx");
    // vertices = gem_reader.readVertices();
    // indices = gem_reader.readIndices();
    // gem_data.position = {-26, 20, -8};
    // gem_data.mesh =
    //     renderer->addMesh(vertices.data(), vertices.size() * sizeof(Vertex),
    //                       indices.data(), indices.size() * sizeof(uint32_t));
    // data = stbi_load("./Assets/gem2.png", &width, &height, &channels, 0);
    // gem_data.albedo = renderer->addTexture(data, width, height);
    // renderer->getRenderWorld().addOpaqueObject(gem_data);

    // OpaqueRenderObjectData terrain_data = {};
    // auto terrain_reader = File::ModelReader("./Assets/Island.fbx");
    // vertices = terrain_reader.readVertices();
    // indices = terrain_reader.readIndices();
    // terrain_data.position = {0, -50, 0};
    // terrain_data.mesh =
    //     renderer->addMesh(vertices.data(), vertices.size() * sizeof(Vertex),
    //                       indices.data(), indices.size() * sizeof(uint32_t));
    // data =
    //     stbi_load("./Assets/island_albedo.png", &width, &height, &channels,
    //     0);
    // terrain_data.albedo = renderer->addTexture(data, width, height);
    // renderer->getRenderWorld().addOpaqueObject(terrain_data);

    auto input = std::make_shared<Input::GameInputContext>();
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

    sky.draw();
    world.update(deltaTime);
}

void Scene::setupSystems() {
    world.addSystem<TransformSystem>();
    world.addSystem<ScriptSystem>();
}
