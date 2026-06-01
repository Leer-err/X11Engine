struct CameraData
{
    matrix view_projection;
    matrix inverse_view_projection;
};

struct StaticModelData
{
    float4x4 model;

    uint32_t albedo_descriptor;
    uint32_t sampler_descriptor;
}

struct Vertex_Output{
    float4 viewport_position : SV_POSITION;
    float2 uv;
};

struct Vertex_input{
    float3 position;
    float2 uv;
};

struct PushConstants{
    CameraData* camera_data;
    StaticModelData* model_data;
};

[vk::push_constant]
PushConstants constants;

[shader("vertex")]
Vertex_Output vertex_main(Vertex_input vertex)
{
    Vertex_Output output;

    float4 vertex_position = float4(vertex.position, 1);

    float4x4 mvp = mul(constants.camera_data->view_projection, constants.model_data->model);
    output.viewport_position = mul(mvp, vertex_position);
    output.uv = vertex.uv;

    return output;
}

[[vk::binding(0, 0)]]
Texture2D textures[];
[[vk::binding(1, 0)]]
SamplerState samplers[];

[shader("pixel")]
float4 pixel_main(in Vertex_Output data)
{
    uint32_t texture_index = constants.model_data.albedo_descriptor;
    uint32_t sampler_index = constants.model_data.sampler_descriptor;
    return textures[texture_index].Sample(samplers[sampler_index], data.uv);
}