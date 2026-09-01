import camera;

struct StaticModelData
{
    float4x4 model;

    uint32_t albedo_descriptor;
    uint32_t sampler_descriptor;
};

struct Meshlet{
    uint32_t vertex_offset; 
    uint32_t triangle_offset;

    uint8_t vertex_count;
    uint8_t triangle_count;
};

struct VertexOutput{
    float4 viewport_position : SV_POSITION;
    float2 uv : TEXCOORD;
};

struct VertexInput{
    float3 position;
    float2 uv;
};

struct MeshData{
    VertexInput* vertices;
    uint8_t* meshlet_triangles;
    uint32_t* meshlet_vertices;
    Meshlet* meshlets;
};

struct PushConstants{
    MeshData mesh;

    CameraData* camera_data;
    StaticModelData* model_data;
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
    Meshlet meshlet = constants.mesh.meshlets[group_index.x];
    SetMeshOutputCounts(meshlet.vertex_count, meshlet.triangle_count);

    float4x4 mvp = mul(constants.camera_data->view_projection, constants.model_data->model);

    uint local_index = group_tid.x;
    for(int i = local_index; i < meshlet.vertex_count; i += GROUP_SIZE){
        uint meshlet_vertex_index = meshlet.vertex_offset + i;
        uint vertex_index = constants.mesh.meshlet_vertices[meshlet_vertex_index];
        VertexInput vertex = constants.mesh.vertices[vertex_index];
        float4 vertex_position = float4(vertex.position, 1);

        vertices[i].viewport_position = mul(mvp, vertex_position);
        vertices[i].uv = vertex.uv;
    }

    for(int i = local_index; i < meshlet.triangle_count; i += GROUP_SIZE){
        uint meshlet_triangle_index = meshlet.triangle_offset + i * 3;

        uint3 triangle;
        triangle.x = constants.mesh.meshlet_triangles[meshlet_triangle_index];
        triangle.y = constants.mesh.meshlet_triangles[meshlet_triangle_index + 1];
        triangle.z = constants.mesh.meshlet_triangles[meshlet_triangle_index + 2];

        triangles[i] = triangle;
    }
}

[[vk::binding(0, 0)]]
Texture2D textures[];
[[vk::binding(1, 0)]]
SamplerState samplers[];

[shader("pixel")]
float4 pixel_main(in float2 uv : TEXCOORD) : SV_Target
{
    uint32_t texture_index = constants.model_data.albedo_descriptor;
    uint32_t sampler_index = constants.model_data.sampler_descriptor;
    return textures[texture_index].Sample(samplers[sampler_index], uv);
}