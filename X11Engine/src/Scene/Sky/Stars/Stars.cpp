#include "Stars.h"

#include "Binding.h"
#include "BufferBuilder.h"
#include "Context.h"
#include "Engine.h"
#include "GraphicsPipelineBuilder.h"
#include "InputLayoutBuilder.h"
#include "Matrix.h"
#include "MeshBuilder.h"
#include "PixelShaderBuilder.h"
#include "Vector3.h"
#include "VertexShaderBuilder.h"

namespace StarData {
static constexpr Binding camera_parameters =
    Binding(Binding::ShaderType::Vertex, 0);
static constexpr Binding star_parameters =
    Binding(Binding::ShaderType::Pixel, 0);

struct CameraParameters {
    Matrix inverse_projection;
};

struct StarParameters {
    float time;

    float padding[3];
};

};  // namespace StarData

Stars::Stars() {
    constexpr Vector3 screen_quad_vertices[] = {
        Vector3(-1, -1, 1), Vector3(1, -1, 1), Vector3(-1, 1, 1),
        Vector3(1, 1, 1)};

    constexpr uint32_t screen_quad_indices[] = {0, 2, 1, 1, 2, 3};

    screen_plane =
        MeshBuilder()
            .addVertexData(&screen_quad_vertices[0],
                           sizeof(screen_quad_vertices), sizeof(Vector3))
            .setIndexData(&screen_quad_indices[0], sizeof(screen_quad_indices))
            .create();

    auto vertex_shader =
        VertexShaderBuilder("./Assets/Shaders/Stars/Stars.hlsl", "vertex_main")
            .create()
            .getResult();
    auto pixel_shader =
        PixelShaderBuilder("./Assets/Shaders/Stars/Stars.hlsl", "pixel_main")
            .create()
            .getResult();

    auto input_layout = InputLayoutBuilder(vertex_shader)
                            .addElement("POSITION", DXGI_FORMAT_R32G32B32_FLOAT)
                            .create();

    pipeline =
        GraphicsPipelineBuilder(input_layout, vertex_shader, pixel_shader)
            .create();

    camera_parameters_buffer = BufferBuilder(sizeof(StarData::CameraParameters))
                                   .isConstantBuffer()
                                   .isCPUWritable()
                                   .create()
                                   .getResult();
    star_parameters_buffer = BufferBuilder(sizeof(StarData::StarParameters))
                                 .isConstantBuffer()
                                 .isCPUWritable()
                                 .create()
                                 .getResult();

    auto context = Context();

    auto camera_parameters =
        context.mapConstantBuffer<StarData::CameraParameters>(
            camera_parameters_buffer);
    camera_parameters->inverse_projection =
        Matrix::projection(60, 16.f / 9, 1000, 0.01).inverse();
    context.unmapConstantBuffer(camera_parameters_buffer);

    auto star_parameters = context.mapConstantBuffer<StarData::StarParameters>(
        star_parameters_buffer);
    star_parameters->time = 0;
    context.unmapConstantBuffer(star_parameters_buffer);
}

void Stars::draw() {
    auto context = Context();

    auto star_parameters = context.mapConstantBuffer<StarData::StarParameters>(
        star_parameters_buffer);
    star_parameters->time = Engine::Engine::get().getTime();
    context.unmapConstantBuffer(star_parameters_buffer);

    context.cleanPipeline();
    context.setPipeline(pipeline);
    context.bindConstantBuffer(camera_parameters_buffer,
                               StarData::camera_parameters);
    context.bindConstantBuffer(star_parameters_buffer,
                               StarData::star_parameters);
    context.draw(screen_plane);
}
