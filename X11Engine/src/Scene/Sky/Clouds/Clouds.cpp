#include "Clouds.h"

#include "BufferBuilder.h"
#include "Context.h"
#include "Engine.h"
#include "Format.h"
#include "GraphicsPipelineBuilder.h"
#include "InputLayoutBuilder.h"
#include "Matrix.h"
#include "MeshBuilder.h"
#include "Overlay.h"
#include "PixelShaderBuilder.h"
#include "SamplerBuilder.h"
#include "ShaderResource.h"
#include "TextureBuilder.h"
#include "Vector2.h"
#include "Vector3.h"
#include "VertexShaderBuilder.h"

namespace CloudBakeData {
static constexpr Binding cloud_parameters{Binding::ShaderType::Pixel, 0};

struct CloudParameters {
    Vector3 color;
    float time;
};

};  // namespace CloudBakeData

Clouds::CloudsBaker::CloudsBaker() {
    constexpr Vector3 screen_quad_vertices[] = {
        Vector3(-1, -1, 1), Vector3(1, -1, 1), Vector3(-1, 1, 1),
        Vector3(1, 1, 1)};

    constexpr uint32_t screen_quad_indices[] = {0, 2, 1, 1, 2, 3};

    screen_quad =
        MeshBuilder()
            .addVertexData(&screen_quad_vertices[0],
                           sizeof(screen_quad_vertices), sizeof(Vector3))
            .setIndexData(&screen_quad_indices[0], sizeof(screen_quad_indices))
            .create();

    auto vertex_shader =
        VertexShaderBuilder("./Assets/Shaders/Sky/SkyPrerender.hlsl",
                            "vertex_main")
            .create()
            .getResult();
    auto pixel_shader =
        PixelShaderBuilder("./Assets/Shaders/Sky/SkyPrerender.hlsl",
                           "pixel_main")
            .create()
            .getResult();

    auto input_layout = InputLayoutBuilder(vertex_shader)
                            .addElement("POSITION", DXGI_FORMAT_R32G32B32_FLOAT)
                            .create();

    clouds_texture = TextureBuilder(ImageFormat::RGBA_32BPP, 512, 512)
                         .isRenderTarget()
                         .isShaderResource()
                         .isGPUWritable()
                         .create()
                         .getResult();

    cloud_render_target = RenderTarget(clouds_texture);

    bake_pipeline =
        GraphicsPipelineBuilder(input_layout, vertex_shader, pixel_shader)
            .setRenderTarget(cloud_render_target)
            .create();

    cloud_parameter_buffer =
        BufferBuilder(sizeof(CloudBakeData::CloudParameters))
            .isConstantBuffer()
            .isCPUWritable()
            .create()
            .getResult();

    auto context = Context();

    auto cloud_parameters =
        context.mapConstantBuffer<CloudBakeData::CloudParameters>(
            cloud_parameter_buffer);
    cloud_parameters->color = Vector3(1, 0, 1);
    cloud_parameters->time = 0;
    context.unmapConstantBuffer(cloud_parameter_buffer);
}

void Clouds::CloudsBaker::draw() {
    auto context = Context();

    auto cloud_parameters =
        context.mapConstantBuffer<CloudBakeData::CloudParameters>(
            cloud_parameter_buffer);
    cloud_parameters->color = Vector3(1, 0, 1);
    cloud_parameters->time = Engine::Engine::get().getTime();
    context.unmapConstantBuffer(cloud_parameter_buffer);

    context.clean(cloud_render_target);

    context.cleanPipeline();
    context.setPipeline(bake_pipeline);
    context.bindConstantBuffer(cloud_parameter_buffer,
                               CloudBakeData::cloud_parameters);
    context.draw(screen_quad);
}

Texture Clouds::CloudsBaker::getCloudsTexture() const { return clouds_texture; }

namespace SkyPipelineData {
static constexpr Binding camera_data{Binding::ShaderType::Vertex, 0};
static constexpr Binding sky_data{Binding::ShaderType::Vertex, 1};
static constexpr Binding cloud_texture{Binding::ShaderType::Pixel, 0};
static constexpr Binding cloud_sampler{Binding::ShaderType::Pixel, 0};

struct CameraData {
    Matrix projection_matrix;
};

struct SkyData {
    float height;
    float cloud_plane_scale;

    float padding[2];
};

};  // namespace SkyPipelineData

struct Vertex {
    Vector3 position;
    Vector2 uv;
};

Clouds::Clouds() : cloud_height(100), cloud_plane_size(1000) {
    constexpr Vertex cloud_plane_vertices[] = {
        Vertex(Vector3(-1, 1, -1), Vector2(0, 1)),
        Vertex(Vector3(-1, 1, 1), Vector2(0, 0)),
        Vertex(Vector3(1, 1, -1), Vector2(1, 1)),
        Vertex(Vector3(1, 1, 1), Vector2(1, 0))};

    constexpr uint32_t cloud_plane_indices[] = {0, 2, 1, 1, 2, 3};

    cloud_plane =
        MeshBuilder()
            .addVertexData(&cloud_plane_vertices[0],
                           sizeof(cloud_plane_vertices), sizeof(Vertex))
            .setIndexData(&cloud_plane_indices[0], sizeof(cloud_plane_indices))
            .create();

    auto vertex_shader =
        VertexShaderBuilder("./Assets/Shaders/Sky/Sky.hlsl", "vertex_main")
            .create()
            .getResult();
    auto pixel_shader =
        PixelShaderBuilder("./Assets/Shaders/Sky/Sky.hlsl", "pixel_main")
            .create()
            .getResult();

    auto input_layout = InputLayoutBuilder(vertex_shader)
                            .addElement("POSITION", DXGI_FORMAT_R32G32B32_FLOAT)
                            .addElement("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT)
                            .create();

    pipeline =
        GraphicsPipelineBuilder(input_layout, vertex_shader, pixel_shader)
            .create();

    cloud_sampler = SamplerBuilder(Filter::Linear).create();

    camera_data_buffer = BufferBuilder(sizeof(SkyPipelineData::CameraData))
                             .isConstantBuffer()
                             .isCPUWritable()
                             .create()
                             .getResult();

    clouds_data_buffer = BufferBuilder(sizeof(SkyPipelineData::SkyData))
                             .isConstantBuffer()
                             .isCPUWritable()
                             .create()
                             .getResult();

    auto context = Context();

    auto camera_data = context.mapConstantBuffer<SkyPipelineData::CameraData>(
        camera_data_buffer);
    camera_data->projection_matrix =
        Matrix::projection(60, 16.f / 9, 1000, 0.01);
    context.unmapConstantBuffer(camera_data_buffer);

    auto sky_data =
        context.mapConstantBuffer<SkyPipelineData::SkyData>(clouds_data_buffer);
    sky_data->height = 100;
    sky_data->cloud_plane_scale = 1000;
    context.unmapConstantBuffer(clouds_data_buffer);

    Overlay::Overlay::get().add<Overlay::OverlayElements::SliderFloat>(
        "Clouds", "Height", [this](float height) { updateHeight(height); }, 0.f,
        300.f, cloud_height);

    Overlay::Overlay::get().add<Overlay::OverlayElements::SliderFloat>(
        "Clouds", "Cloud plane size",
        [this](float value) { updateCloudPlaneSize(value); }, 0.f, 2000.f,
        cloud_plane_size);
}

void Clouds::draw() {
    auto context = Context();

    cloud_baker.draw();
    cloud_texture = ShaderResource(cloud_baker.getCloudsTexture());

    context.cleanPipeline();

    context.setPipeline(pipeline);
    context.bindShaderResource(cloud_texture, SkyPipelineData::cloud_texture);
    context.bindSampler(cloud_sampler, SkyPipelineData::cloud_sampler);
    context.bindConstantBuffer(camera_data_buffer,
                               SkyPipelineData::camera_data);
    context.bindConstantBuffer(clouds_data_buffer, SkyPipelineData::sky_data);
    context.draw(cloud_plane);
}

void Clouds::updateCloudData() {
    auto context = Context();

    auto sky_data =
        context.mapConstantBuffer<SkyPipelineData::SkyData>(clouds_data_buffer);
    sky_data->height = cloud_height;
    sky_data->cloud_plane_scale = cloud_plane_size;
    context.unmapConstantBuffer(clouds_data_buffer);
}

void Clouds::updateHeight(float height) {
    this->cloud_height = height;

    updateCloudData();
}

void Clouds::updateCloudPlaneSize(float cloud_plane_size) {
    this->cloud_plane_size = cloud_plane_size;

    updateCloudData();
}