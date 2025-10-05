cbuffer CameraData : register(b0)
{
    float4x4 view_projection;
};

cbuffer ModelData : register(b1)
{
    float4x4 world;
};

struct Output{
    float3 position : POSITION;
    float4 viewport_position : SV_POSITION;
    float2 uv : TEXCOORD;
};

struct Vertex{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;

    int bone_ids[4] : BONES;
    float bone_weights[4] : WEIGHTS;
};

Output main(in Vertex vertex)
{
    float4x4 world_view_projection = mul(world, view_projection);

    Output output;
    output.position = vertex.position;
    output.viewport_position = mul(float4(vertex.position, 1.f),  world_view_projection);
    output.uv = vertex.uv;
    return output;
}