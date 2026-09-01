import camera;

struct CloudsData
{
    float3 cloud_color;
    float time;
    float height;
    float cloud_plane_scale;
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
    CloudsData* cloud_data;
};

[vk::push_constant]
PushConstants constants;

[shader("vertex")]
Vertex_Output vertex_main(Vertex_input vertex)
{
    Vertex_Output output;

    float4 vertex_position = float4(vertex.position, 1);
    vertex_position.xz = vertex_position.xz * constants.cloud_data->cloud_plane_scale;
    vertex_position.y = constants.cloud_data->height;
    output.viewport_position = mul(constants.camera_data.view_projection, vertex_position);
    output.uv = vertex.uv;

    return output;
}

[[vk::binding(0, 0)]]
Texture2D clouds;
[[vk::binding(1, 0)]]
SamplerState clouds_sampler;

[shader("pixel")]
float4 pixel_main(in Vertex_Output data)
{
    return clouds.Sample(clouds_sampler, data.uv);
}