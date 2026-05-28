struct Vertex_Output {
    float4 viewport_position : SV_Position;
    float2 uv;
};

struct Vertex_input {
    float3 position;
};

struct DitheringData {
    int2 camera_dimensions;
    float spread;
    uint32_t color_count;
    uint32_t texture_index;
    uint32_t sampler_index;
};

static const float2x2 thresholds = {{0,0.5},{0.75,0.25}};

struct PushConstants{
    DitheringData* parameters;
};

[vk::push_constant]
PushConstants constants;

[shader("vertex")]
Vertex_Output vertex_main(in Vertex_input vertex)
{
    Vertex_Output output;
    output.viewport_position = float4(vertex.position, 1);
    output.uv = (vertex.position.xy + 1) / 2;

    return output;
}

[[vk::binding(0, 0)]]
Texture2D textures[];
[[vk::binding(1, 0)]]
SamplerState samplers;

float3 quantize_color(float3 color) {
    return floor(color * (constants.parameters->color_count - 1) + 0.5) / (constants.parameters->color_count - 1);
}

[shader("pixel")]
float4 pixel_main(in Vertex_Output data)
{
    int2 pixel_coordinates = data.uv * constants.parameters->camera_dimensions;
    int2 threshold_coordinates = pixel_coordinates % int2(2, 2);

    float noise = thresholds[threshold_coordinates.x][threshold_coordinates.y] - 0.5;

    float4 color = textures[constants.parameters->texture_index].Sample(samplers, data.uv) + noise * constants.parameters->spread;

    return float4(quantize_color(color.xyz), 1);
}