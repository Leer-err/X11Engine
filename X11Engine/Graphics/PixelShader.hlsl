cbuffer lightning : register(b0)
{
    float4 ambientColor;
}

struct input
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 lightDir : LIGHT;
};

Texture2D tex : register(t0);
SamplerState samp : register(s0);

float4 main(input in_data) : SV_TARGET
{
    float4 diff = max(dot(in_data.normal, in_data.lightDir), 0.f) * float4(1.f, 1.f, 1.f, 1.f);
    return tex.Sample(samp, in_data.uv) * (ambientColor + diff);
}