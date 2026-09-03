#include "Scene.h"

#include <stb_image.h>

#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <tracy/Tracy.hpp>

#include "Camera.h"
#include "EffectDescription.h"
#include "Emitter.h"
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

void from_json(const nlohmann::json& j, Vector3& vec) {
    j.at("x").get_to(vec.x);
    j.at("y").get_to(vec.y);
    j.at("z").get_to(vec.z);
}

void from_json(const nlohmann::json& j, Vector4& vec) {
    j.at("x").get_to(vec.x);
    j.at("y").get_to(vec.y);
    j.at("z").get_to(vec.z);
    j.at("w").get_to(vec.w);
}

namespace Graphics {
void from_json(const nlohmann::json& j, BoxSpawner& spawner) {
    j.at("center").get_to(spawner.center);
    j.at("extents").get_to(spawner.extents);
}
}  // namespace Graphics

Scene::Scene() {
    setupSystems();

    camera = Camera::create(60, 16.f / 9, 0.1, 1000);

    auto renderer = Graphics::getRenderEngine();

    int width;
    int height;
    int channels;
    unsigned char* data;

    RenderObjectData tower_data =
        *readRenderObject("./Assets/Scene/Tower.json");
    renderer->getRenderWorld().addOpaqueObject(tower_data);

    RenderObjectData gem_data = *readRenderObject("./Assets/Scene/Gem.json");
    renderer->getRenderWorld().addOpaqueObject(gem_data);

    RenderObjectData island_data =
        *readRenderObject("./Assets/Scene/Island.json");
    renderer->getRenderWorld().addOpaqueObject(island_data);

    auto sparkles = *readEffect("./Assets/Scene/Effects/Sparkles.json");
    renderer->getRenderWorld().addEffect(sparkles);
    auto orb = *readEffect("./Assets/Scene/Effects/Orb.json");
    renderer->getRenderWorld().addEffect(orb);

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

std::optional<TextureHandle> Scene::readTexture(
    const std::filesystem::path& path) {
    int width;
    int height;
    int channels;
    unsigned char* texture_data;

    auto renderer = Graphics::getRenderEngine();
    texture_data =
        stbi_load(path.string().c_str(), &width, &height, &channels, 0);
    return renderer->addTexture(texture_data, width, height);
}

std::optional<RenderObjectData> Scene::readRenderObject(
    const std::filesystem::path& path) {
    auto file = std::ifstream(path);

    auto data = nlohmann::json();
    file >> data;

    auto mesh_path = data["mesh"].get<std::string>();
    auto albedo_path = data["albedo"].get<std::string>();
    auto position = data["position"].get<Vector3>();

    auto mesh_data = File::ModelReader(mesh_path).readMesh();
    auto renderer = Graphics::getRenderEngine();
    auto albedo = *readTexture(albedo_path);
    auto mesh = renderer->addMesh(mesh_data);

    return RenderObjectData{position, albedo, mesh};
}

std::optional<Graphics::EffectDescription> Scene::readEffect(
    const std::filesystem::path& path) {
    auto file = std::ifstream(path);

    auto data = nlohmann::json();
    file >> data;

    auto effect = Graphics::EffectDescription{};
    for (const auto emitter_data : data["emitters"]) {
        auto spawner = emitter_data["spawner"].get<Graphics::BoxSpawner>();
        auto spawn_rate = emitter_data["spawn_rate"].get<float>();
        auto color = emitter_data["color"].get<Vector4>();
        auto size = emitter_data["size"].get<float>();
        auto particle_lifetime = emitter_data["particle_lifetime"].get<float>();
        auto texture_path = emitter_data["texture"].get<std::string>();
        auto texture = *readTexture(texture_path);

        effect.emitters.emplace_back(spawner, spawn_rate, color, size,
                                     particle_lifetime, texture);
    }

    return effect;
}
