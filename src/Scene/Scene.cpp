#include "Scene.h"

#include <stb_image.h>

#include <tracy/Tracy.hpp>

#include "Graphics.h"
#include "GraphicsCommunicationManager.h"
#include "ModelReader.h"
#include "Quaternion.h"
#include "StaticModelData.h"
#include "Camera.h"
#include "Vector3.h"

StaticModelData model_data = {};

Scene::Scene() {
    camera = Camera::create(110, 16.f / 9, 1, 1000);

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
}

void Scene::update(float deltaTime) {
    ZoneScoped;
    auto& graphics_communicator = GraphicsCommunicationManager::get();

    sky.draw();
    world.update(deltaTime);

    graphics_communicator.send(model_data);
}

void Scene::setupSystems() {}
