import mesh;

static const float2x2 bayer_2x2 = { {0, 2}, {3, 1} };

static const float4x4 bayer_4x4 = { {0, 8, 2, 10},
                                    {12, 4, 14, 6},
                                    { 3, 11, 1, 9},
                                    {15, 7, 13, 5} };

static const float bayer_8x8[8][8] = { { 0, 48, 12, 60,  3, 51, 15, 63},
                                               {32, 16, 44, 28, 35, 19, 47, 31},
                                               { 8, 56,  4, 52, 11, 59,  7, 55},
                                               {40, 24, 36, 20, 43, 27, 39, 23},
                                               { 2, 50, 14, 62,  1, 49, 13, 61},
                                               {34, 18, 46, 30, 33, 17, 45, 29},
                                               {10, 58,  6, 54,  9, 57,  5, 53},
                                               {42, 26, 38, 22, 41, 25, 37, 21} };

float3 dithering2x2(float3 color, float spread, uint2 pixel_coordinates){
    static const uint SIZE = 2;

    int2 threshold_coordinates = pixel_coordinates % int2(SIZE, SIZE);

    float noise = bayer_2x2[threshold_coordinates.x][threshold_coordinates.y] / (SIZE * SIZE) - 0.5;

    return color + noise * spread;
}

float3 dithering4x4(float3 color, float spread, uint2 pixel_coordinates){
    static const uint SIZE = 4;

    int2 threshold_coordinates = pixel_coordinates % int2(SIZE, SIZE);

    float noise = bayer_4x4[threshold_coordinates.x][threshold_coordinates.y] / (SIZE * SIZE) - 0.5;

    return color + noise * spread;
}

float3 dithering8x8(float3 color, float spread, uint2 pixel_coordinates){
    static const uint SIZE = 8;

    int2 threshold_coordinates = pixel_coordinates % int2(SIZE, SIZE);

    float noise = bayer_8x8[threshold_coordinates.x][threshold_coordinates.y] / (SIZE * SIZE) - 0.5;

    return color + noise * spread;
}

struct VertexOutput {
    float4 viewport_position : SV_Position;
    float2 uv : TEXCOORD;
};

struct VertexInput {
    float3 position;
    float2 uv;
};

struct PostProcessingData {
    int2 camera_dimensions;
    uint32_t texture_index;
    uint32_t sampler_index;
    
    //dithering
    float spread;
    // uint32_t dithering_order;

    //color quantization
    uint32_t color_count;

    // bool enable_dithering;
    // bool enable_color_quantization;
};

struct PushConstants{
    Mesh<VertexInput> mesh;

    PostProcessingData* parameters;
};

[vk::push_constant]
PushConstants constants;

static const uint GROUP_SIZE = 32;

[shader("mesh")]
[outputtopology("triangle")]
[numthreads(GROUP_SIZE, 1, 1)]
void mesh_main( 
    in uint3 group_tid : SV_GroupThreadID,
    in uint3 group_index : SV_GroupID,
    out vertices VertexOutput vertices[64],
    out indices uint3 triangles[96])
{
    let meshlet_index = group_index.x;
    Meshlet meshlet = constants.mesh.meshlets[meshlet_index];
    SetMeshOutputCounts(meshlet.vertex_count, meshlet.triangle_count);

    for(int i = group_tid.x; i < meshlet.vertex_count; i += GROUP_SIZE){
        VertexInput vertex = constants.mesh.getVertex(meshlet_index, i);

        float4 vertex_position = float4(vertex.position, 1);

        vertices[i].viewport_position = float4(vertex.position.xy, 1, 1);
        vertices[i].uv = vertex.uv;
    }

    for(int i = group_tid.x; i < meshlet.triangle_count; i += GROUP_SIZE){
        triangles[i] = constants.mesh.getTriangle(meshlet_index, i);
    }
}

[[vk::binding(0, 0)]]
Texture2D textures[];
[[vk::binding(1, 0)]]
SamplerState samplers;

float3 quantize_color(float3 color) {
    return floor(color * (constants.parameters->color_count - 1) + 0.5) / (constants.parameters->color_count - 1);
}

[shader("pixel")]
float4 pixel_main(in VertexOutput data)
{
    int2 pixel_coordinates = data.uv * constants.parameters->camera_dimensions;

    float3 color = textures[constants.parameters->texture_index].Sample(samplers, data.uv).rgb;

    color = dithering8x8(color, constants.parameters->spread, pixel_coordinates);

    return float4(quantize_color(color.xyz), 1);
}