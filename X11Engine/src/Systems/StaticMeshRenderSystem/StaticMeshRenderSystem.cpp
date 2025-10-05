#include "StaticMeshRenderSystem.h"

#include <memory>
#include <tracy/Tracy.hpp>
#include <vector>

#include "Entity.h"
#include "ICamera.h"
#include "IShaderBlob.h"
#include "IShaderReader.h"
#include "LoggerFactory.h"
#include "Matrix.h"
#include "Model.h"
#include "PlayerComponent.h"
#include "PositionComponents.h"
#include "ShaderType.h"
#include "StaticMesh.h"
#include "StaticProjectionCamera.h"
#include "World.h"

StaticMeshRenderSystem::StaticMeshRenderSystem(
    std::shared_ptr<IResourceFactory> factory,
    std::shared_ptr<IShaderReader> shader_reader)
    : context(factory->getContext()),
      factory(factory),
      shader_reader(shader_reader),
      logger(LoggerFactory::getLogger("StaticMeshRenderSystem")) {}

bool StaticMeshRenderSystem::prepare() {
    constexpr char PIXEL_SHADER_FILE[] =
        "E:\\repos\\X11Engine\\X11Engine\\src\\Shaders\\PixelShader.hlsl";
    constexpr char VERTEX_SHADER_FILE[] =
        "E:\\repos\\X11Engine\\X11Engine\\src\\Shaders\\VertexShader.hlsl";

    std::shared_ptr<IShaderBlob> pixel_shader_source =
        shader_reader->readPixelShader(PIXEL_SHADER_FILE);
    pixel_shader = factory->createPixelShader(pixel_shader_source.get());

    if (pixel_shader == nullptr) {
        logger.error("Pixel shader was not loaded");
        return false;
    }

    std::shared_ptr<IShaderBlob> vertex_shader_source =
        shader_reader->readVertexShader(VERTEX_SHADER_FILE);
    vertex_shader = factory->createVertexShader(vertex_shader_source.get());

    if (vertex_shader == nullptr) {
        logger.error("Pixel shader was not loaded");
        return false;
    }

    camera_buffer =
        factory->createConstantBuffer(nullptr, sizeof(Matrix), true);
    world_matrix_buffer =
        factory->createConstantBuffer(nullptr, sizeof(Matrix), true);

    return true;
}

void StaticMeshRenderSystem::update(World& world, float delta_time) {
    ZoneScoped;

    context->setPixelShader(pixel_shader);
    context->setVertexShader(vertex_shader);

    std::vector<Entity> players = world.query().with<Player>().execute();
    std::vector<Entity> cameras = world.query()
                                      .childOf(players[0])
                                      .with<StaticProjectionCamera>()
                                      .with<GlobalMatrix>()
                                      .execute();
    auto camera = (const ICamera*)cameras[0].get<StaticProjectionCamera>();
    auto camera_transform = cameras[0].get<GlobalMatrix>();

    auto camera_world_matrix = camera_transform->matrix.inverse();
    auto camera_projection_matrix = camera->getProjectionMatrix();

    Matrix camera_matrix = camera_world_matrix * camera_projection_matrix;

    camera_buffer->update((char*)&camera_matrix, sizeof(Matrix), 0);
    context->bindConstantBuffer(ShaderType::Vertex, camera_buffer, 0);

    std::vector<Entity> entities =
        world.query().with<StaticMesh>().with<GlobalMatrix>().execute();
    for (auto& entity : entities) {
        const Model model = entity.get<StaticMesh>()->model;
        const GlobalMatrix* transform = entity.get<GlobalMatrix>();

        const Material& material = model.material;

        context->bindTexture(ShaderType::Pixel, material.albedo_texture, 0);

        world_matrix_buffer->update((char*)&transform->matrix, sizeof(Matrix),
                                    0);
        context->bindConstantBuffer(ShaderType::Vertex, world_matrix_buffer, 1);

        for (const Mesh& mesh : model.meshes) {
            context->setVertexBuffer(mesh.vertex_buffer);
            context->setIndexBuffer(mesh.index_buffer);

            context->draw(mesh.index_buffer->getIndexCount());
        }
    }
}
