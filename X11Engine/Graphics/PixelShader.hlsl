struct input
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D tex : register(t0);
SamplerState samp : register(s0);

float4 main(input in_data) : SV_TARGET
{
    return tex.Sample(samp, in_data.uv);
}