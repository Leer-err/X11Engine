cbuffer data : register(b0)
{
    matrix projection;
};

struct output
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

output main(float4 pos : POSITION)
{
    output a;
    a.pos = mul(pos, projection);
    a.color = pos;
    return a;
}