cbuffer CameraData : register(b0)
{
    float4x4 view_projection;
};

cbuffer ModelData : register(b1)
{
    float4x4 world;
};

StructuredBuffer<float4x4> bones;

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

static const int MAX_BONES_PER_VERTEX = 4;

Output main(in Vertex vertex)
{
    float4 final_position = float4(0, 0, 0, 0);
    [unroll(4)]
    for(int i = 0; i < MAX_BONES_PER_VERTEX; i++){
        int bone_id = vertex.bone_ids[i];

        float4 local_position = mul(bones[bone_id], float4(vertex.position, 1));
        final_position += local_position * vertex.bone_weights[i];
    }

    float4x4 world_view_projection = mul(view_projection, world);

    Output output;
    output.position = final_position.xyz;
    output.viewport_position = mul(world_view_projection, final_position);
    output.uv = vertex.uv;
    return output;
}