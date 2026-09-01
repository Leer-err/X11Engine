import camera;

struct StaticModelData
{
    float4x4 model;

    uint32_t albedo_descriptor;
    uint32_t sampler_descriptor;
}

struct Meshlet{
    uint32_t vertex_offset; 
    uint32_t triangle_offset;

    uint8_t vertex_count;
    uint8_t triangle_count;
};

struct VertexOutput{
    float4 viewport_position : SV_POSITION;
    float2 uv : TEXCOORD;
    uint32_t texture_index : MATERIAL;
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
    float3* positions_data;
    uint32_t* materials_data;
    uint64_t* live_particles;
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

    uint local_index = group_tid.x;
    for(int i = local_index; i < meshlet.vertex_count; i += GROUP_SIZE){
        uint meshlet_vertex_index = meshlet.vertex_offset + i;
        uint vertex_index = constants.mesh.meshlet_vertices[meshlet_vertex_index];
        VertexInput vertex = constants.mesh.vertices[vertex_index];

        uint64_t particle = constants.live_particles[group_index.y];
        float3 world_positon = constants.positions_data[particle] + (vertex.position.y * constants.camera_data.up + vertex.position.x * constants.camera_data.right) * 0.5;
        float4 vertex_position = float4(world_positon, 1);

        vertices[i].viewport_position = mul(constants.camera_data.view_projection, vertex_position);
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

static float4 particle_color = float4(0.475, 0.173, 0.635, 1);

[shader("pixel")]
float4 pixel_main(in float2 uv : TEXCOORD, in uint32_t texture_index : MATERIAL) : SV_Target
{
    uint32_t texture_index = constants.materials_data[texture_index];
    uint32_t sampler_index = 0;
    return textures[texture_index].Sample(samplers[sampler_index], uv) * particle_color;
}