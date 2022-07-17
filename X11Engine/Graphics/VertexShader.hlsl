cbuffer windowData : register(b0)
{
    matrix projection;
}

cbuffer frameData : register(b1)
{
    matrix view;
    float3 lightPos;
};

cbuffer modelData : register(b2)
{
    matrix world;
};


struct output
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 lightDir : LIGHT;
};

struct input
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

output main(input in_data)
{
    output a;
    float4 pos = float4(in_data.pos, 1.f);
    a.pos = mul(pos, world);
    a.lightDir = normalize(lightPos - a.pos.xyz);
    a.pos = mul(a.pos, view);
    a.pos = mul(a.pos, projection);
    a.uv = in_data.uv;
    a.normal = in_data.normal;
    return a;
}