import camera;
import mesh;

struct StaticModelData
{
    float4x4 model;

    uint32_t albedo_descriptor;
    uint32_t sampler_descriptor;
}

struct VertexOutput{
    float4 viewport_position : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 color : COLOR0;
    uint32_t texture_index : MATERIAL;
};

struct VertexInput{
    float3 position;
    float2 uv;
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
    Mesh<VertexInput> mesh;

    CameraData* camera_data;
    Particle* particles_data;
    uint32_t* live_particles_data;
};

[vk::push_constant]
PushConstants constants;

float2x2 rotate(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return float2x2(c, -s, s, c);
}

float4 transformPosition(in float3 local_position, in Particle particle){
    float3 rotated_vertex = float3(mul(rotate(particle.rotation),local_position.xy), 0);

    float3 world_position = particle.position + (rotated_vertex.y * constants.camera_data.up + rotated_vertex.x * constants.camera_data.right) * particle.size;

    return float4(world_position, 1);
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
    let meshlet_index = group_index.x;
    Meshlet meshlet = constants.mesh.meshlets[meshlet_index];
    SetMeshOutputCounts(meshlet.vertex_count, meshlet.triangle_count);

    let particle_index = constants.live_particles_data[group_index.y];
    Particle particle = constants.particles_data[particle_index];

    for(int i = group_tid.x; i < meshlet.vertex_count; i += GROUP_SIZE){
        VertexInput vertex = constants.mesh.getVertex(meshlet_index, i);

        vertices[i].viewport_position = mul(constants.camera_data.view_projection, transformPosition(vertex.position, particle));
        vertices[i].uv = vertex.uv;
        vertices[i].color = particle.color;
        vertices[i].texture_index = particle.texture;
    }

    for(int i = group_tid.x; i < meshlet.triangle_count; i += GROUP_SIZE){
        triangles[i] = constants.mesh.getTriangle(meshlet_index, i);
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