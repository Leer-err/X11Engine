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
    float4 color : COLOR;
};

struct input
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD;
};

output main(input in_data)
{
    output a;
    a.pos = mul(in_data.pos, wvpMatrix);
    a.color = in_data.pos;
    return a;
}