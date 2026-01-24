#include "StaticMeshRenderSystem.h"

#include <memory>
#include <tracy/Tracy.hpp>
#include <vector>

#include "BufferBuilder.h"
#include "CameraManager.h"
#include "CommonSamplers.h"
#include "Context.h"
#include "Entity.h"
#include "GraphicsPipelineBuilder.h"
#include "InputLayoutBuilder.h"
#include "LoggerFactory.h"
#include "Matrix.h"
#include "PixelShaderBuilder.h"
#include "StaticMaterial.h"
#include "StaticMesh.h"
#include "VertexShaderBuilder.h"
#include "World.h"
#include "WorldMatrix.h"

namespace Data = Asset::StaticModel::Data;

using Asset::StaticModel::Material;
using Engine::Graphics::CommonSamplers;

StaticMeshRenderSystem::StaticMeshRenderSystem()
    : logger(LoggerFactory::getLogger("StaticMeshRenderSystem")) {
    auto vertex_shader =
        VertexShaderBuilder("./Assets/Shaders/StaticMesh/StaticMeshVS.hlsl",
                            "main")
            .create()
            .getResult();
    auto pixel_shader =
        PixelShaderBuilder("./Assets/Shaders/StaticMesh/StaticMeshPS.hlsl",
                           "main")
            .create()
            .getResult();

    auto input_layout = InputLayoutBuilder(vertex_shader)
                            .addElement("POSITION", DXGI_FORMAT_R32G32B32_FLOAT)
                            .addElement("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT)
                            .addElement("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT)
                            .create();

    pipeline =
        GraphicsPipelineBuilder(input_layout, vertex_shader, pixel_shader)
            .create();

    world_matrix_buffer = BufferBuilder(sizeof(Data::WorldMatrixBuffer))
                              .isConstantBuffer()
                              .isCPUWritable()
                              .create()
                              .getResult();
}

void StaticMeshRenderSystem::render(World& world) {
    ZoneScoped;

    auto context = Context();
    context.setPipeline(pipeline);

    context.bindConstantBuffer(CameraManager::get().getCameraData(),
                               Data::camera_parameters);

    std::vector<Entity> entities = world.query()
                                       .with<StaticMesh>()
                                       .with<Material>()
                                       .with<WorldMatrix>()
                                       .execute();

    for (auto& entity : entities) {
        auto mesh = entity.get<StaticMesh>()->mesh;
        auto world_matrix = entity.get<WorldMatrix>()->matrix;
        auto materal = entity.get<Material>();
        auto albedo_texture = ShaderResource(materal->albedo);

        auto world_matrix_ptr =
            context.mapConstantBuffer<Data::WorldMatrixBuffer>(
                world_matrix_buffer);
        world_matrix_ptr->world_matrix = world_matrix;
        context.unmapConstantBuffer(world_matrix_buffer);

        context.bindConstantBuffer(world_matrix_buffer, Data::world_matrix);
        context.bindShaderResource(albedo_texture, Data::albedo_texture);

        context.bindSampler(CommonSamplers::linear(), Data::albedo_sampler);

        context.draw(mesh);

        context.unbindConstantBuffer(Data::world_matrix);
        context.unbindShaderResource(Data::albedo_texture);
        context.unbindSampler(Data::albedo_sampler);
    }
}
