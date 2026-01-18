#ifndef STATIC_MESH_RENDER_SYSTEM_H
#define STATIC_MESH_RENDER_SYSTEM_H

#include <memory>

#include "Buffer.h"
#include "GraphicsPipeline.h"
#include "ISystem.h"
#include "Logger.h"

class World;

class StaticMeshRenderSystem : public ISystem {
   public:
    StaticMeshRenderSystem();

    void render(World& world) override;

   private:
    GraphicsPipeline pipeline;

    Buffer world_matrix_buffer;

    Logger logger;
};

// class StaticMeshRenderSystem : public ISystem {
//    public:
//     StaticMeshRenderSystem(std::shared_ptr<IResourceFactory> factory,
//                            std::shared_ptr<IShaderReader> shader_reader);

//     bool prepare(World& world) override;

//     void update(World& world, float delta_time) override;

//     void setCamera(std::shared_ptr<ICamera> camera);
//     void setCameraEntity(Entity camera_entity);

//    private:
//     Entity camera_entity;
//     std::shared_ptr<ICamera> camera;

//     std::shared_ptr<IConstantBuffer> camera_buffer;
//     std::shared_ptr<IConstantBuffer> world_matrix_buffer;

//     std::shared_ptr<IPixelShader> pixel_shader;
//     std::shared_ptr<IVertexShader> vertex_shader;
//     std::shared_ptr<IShaderReader> shader_reader;

//     std::shared_ptr<IRenderContext> context;
//     std::shared_ptr<IResourceFactory> factory;

//     Logger logger;
// };

#endif  // STATIC_MESH_RENDER_SYSTEM_H