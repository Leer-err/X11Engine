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
Texture2D rendered_image;
[[vk::binding(1, 0)]]
SamplerState rendered_image_sampler;

static const int COLOR_COUNT = 32;
float3 quantize_color(float3 color) {
    return floor(color * (COLOR_COUNT - 1) + 0.5) / (COLOR_COUNT - 1);
}

[shader("pixel")]
float4 pixel_main(in Vertex_Output data)
{
    int2 pixel_coordinates = data.uv * constants.parameters->camera_dimensions;
    int2 threshold_coordinates = pixel_coordinates % int2(2, 2);

    float noise = thresholds[threshold_coordinates.x][threshold_coordinates.y] - 0.5;

    float4 color = rendered_image.Sample(rendered_image_sampler, data.uv);// + noise * 0.1;

    return float4(quantize_color(color.xyz), 1);
}