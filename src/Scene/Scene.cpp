#include "Scene.h"

#include <stb_image.h>

#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>
#include <tracy/Tracy.hpp>

#include "Camera.h"
#include "EffectDescription.h"
#include "Entity.h"
#include "GameInputContext.h"
#include "Graphics.h"
#include "LookScript.h"
#include "ModelReader.h"
#include "MoveScript.h"
#include "PhysicalInput.h"
#include "RenderObjectData.h"
#include "ScriptSystem.h"
#include "Transform.h"
#include "TransformSystem.h"
#include "Vector3.h"

Scene::Scene() {
    setupSystems();

    camera = Camera::create(60, 16.f / 9, 0.1, 1000);

    auto renderer = Graphics::getRenderEngine();

    int width;
    int height;
    int channels;
    unsigned char* data;

    RenderObjectData tower_data =
        *readRenderObjectFile("./Assets/Scene/Tower.json");
    renderer->getRenderWorld().addOpaqueObject(tower_data);

    RenderObjectData gem_data =
        *readRenderObjectFile("./Assets/Scene/Gem.json");
    renderer->getRenderWorld().addOpaqueObject(gem_data);

    RenderObjectData island_data =
        *readRenderObjectFile("./Assets/Scene/Island.json");
    renderer->getRenderWorld().addOpaqueObject(island_data);

    auto effect = Graphics::EffectDescription{};
    data = stbi_load("./Assets/star_06.png", &width, &height, &channels, 0);
    auto magic_particle = renderer->addTexture(data, width, height);
    effect.emitters.push_back(Graphics::EmitterDescription{
        {{-26, 20, -8}, {1.5, 2, 1.5}}, 1, 6.8, magic_particle});
    renderer->getRenderWorld().addEffect(effect);

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

    auto renderer = Graphics::getRenderEngine();
    renderer->getRenderWorld().update(deltaTime);
}

void Scene::setupSystems() {
    world.addSystem<TransformSystem>();
    world.addSystem<ScriptSystem>();
}

std::optional<RenderObjectData> Scene::readRenderObjectFile(
    const std::filesystem::path& path) {
    auto file = std::ifstream(path);

    auto data = nlohmann::json();
    file >> data;

    auto mesh_path = data["mesh"].get<std::string>();
    auto albedo_path = data["albedo"].get<std::string>();
    auto position_data = data["position"].get<nlohmann::json>();
    auto position_x = position_data["x"].get<float>();
    auto position_y = position_data["y"].get<float>();
    auto position_z = position_data["z"].get<float>();

    auto mesh_data = File::ModelReader(mesh_path).readMesh();
    auto position = Vector3(position_x, position_y, position_z);
    int width;
    int height;
    int channels;
    unsigned char* texture_data;

    auto renderer = Graphics::getRenderEngine();
    texture_data = stbi_load(albedo_path.data(), &width, &height, &channels, 0);
    auto albedo = renderer->addTexture(texture_data, width, height);
    auto mesh = renderer->addMesh(mesh_data);

    return RenderObjectData{position, albedo, mesh};
}
