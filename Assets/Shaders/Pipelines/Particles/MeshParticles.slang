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
    uint32_t texture_index : MATERIAL;
};

struct VertexInput{
    float3 position;
    float2 uv;
};

struct MeshParticle{
    float3 position;
    float4 orientation;
    float lifetime;
    float age;
    uint32_t texture;
    uint32_t mesh;
}

struct PushConstants{
    Mesh<VertexInput> mesh;

    CameraData* camera_data;
    MeshParticle* particles_data;
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
    let meshlet_index = group_index.x;
    Meshlet meshlet = constants.mesh.meshlets[meshlet_index];
    SetMeshOutputCounts(meshlet.vertex_count, meshlet.triangle_count);

    MeshParticle particle = *constants.particles_data;

    for(int i = group_tid.x; i < meshlet.vertex_count; i += GROUP_SIZE){
        VertexInput vertex = constants.mesh.getVertex(meshlet_index, i);

        vertices[i].viewport_position = mul(constants.camera_data.view_projection, float4(vertex.position, 1));
        vertices[i].uv = vertex.uv;
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
float4 pixel_main(in float2 uv : TEXCOORD, in uint32_t texture_index : MATERIAL) : SV_Target
{
    uint32_t sampler_index = 0;
    return textures[texture_index].Sample(samplers[sampler_index], uv);
}