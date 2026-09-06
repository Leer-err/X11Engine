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
    float4 color : COLOR0;
    uint64_t texture_index : MATERIAL;
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

struct Particle{
    float3 position;
    float3 velocity;
    float4 color;
    float size;
    float rotation;
    float lifetime;
    float age;
    uint32_t texture;
}

struct PushConstants{
    MeshData mesh;

    CameraData* camera_data;
    Particle* particles_data;
    uint64_t* live_particles_data;
};

[vk::push_constant]
PushConstants constants;

float2x2 rotate(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return float2x2(c, -s, s, c);
}

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

        uint64_t particle_index = constants.live_particles_data[group_index.y];
        Particle particle = constants.particles_data[particle_index];
        float3 vertex_local = float3(mul(rotate(particle.rotation), vertex.position.xy), vertex.position.z);

        float3 world_positon = particle.position + (vertex_local.y * constants.camera_data.up + vertex_local.x * constants.camera_data.right) * particle.size;
        float4 vertex_position = float4(world_positon, 1);

        vertices[i].viewport_position = mul(constants.camera_data.view_projection, vertex_position);
        vertices[i].uv = vertex.uv;
        vertices[i].color = particle.color;
        vertices[i].texture_index = particle.texture;
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
float4 pixel_main(in float2 uv : TEXCOORD, in float4 color : COLOR0, in uint32_t texture_index : MATERIAL) : SV_Target
{
    uint32_t sampler_index = 0;
    float4 final_color = color;
    final_color *= textures[texture_index].Sample(samplers[sampler_index], uv);
    return final_color;
}