cbuffer CameraData : register(b0)
{
    float4x4 view_projection;
    float4x4 inverse_view_projection;
};

cbuffer WorldData : register(b1)
{
    float4x4 world;
};

struct Output{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float4 viewport_position : SV_POSITION;
};

struct Vertex{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

Output main(in Vertex vertex)
{
    float4x4 world_view_projection = mul(view_projection, world);

    Output output;
    output.position = vertex.position;
    output.viewport_position = mul(world_view_projection, float4(vertex.position, 1.f));
    output.uv = vertex.uv;
    return output;
}