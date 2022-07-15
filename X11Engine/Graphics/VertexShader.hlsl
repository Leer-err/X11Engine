cbuffer frameData : register(b0)
{
    
};

cbuffer modelData : register(b1)
{
    matrix wvpMatrix;
};

cbuffer drawData : register(b2)
{
    
};


struct output
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
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
    a.pos = mul(pos, wvpMatrix);
    a.uv = in_data.uv;
    return a;
}