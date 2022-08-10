cbuffer windowData : register(b0)
{
    float4x4 projection;
}

cbuffer frameData : register(b1)
{
    float4x4 view;
};

cbuffer modelData : register(b2)
{
    float4x4 world;
    float4x4 normalMatrix;
};


struct output
{
    float4 pos : SV_POSITION;
    float3 fragPos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct input
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

output main(in input in_data)
{
    output a;
    float4 pos = float4(in_data.pos, 1.f);
    float4 normal = float4(in_data.normal, 1.f);
    a.pos = mul(pos, world);
    a.fragPos = a.pos;
    a.pos = mul(a.pos, view);
    a.pos = mul(a.pos, projection);
    a.uv = in_data.uv;
    a.normal = mul(normal, normalMatrix).xyz;
    return a;
}