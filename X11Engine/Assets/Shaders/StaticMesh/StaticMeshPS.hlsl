struct Input{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float4 viewport_position : SV_POSITION;
};

Texture2D albedo : register(t0);
SamplerState albedo_sampler : register(s0);

float4 main(in Input data) : SV_TARGET
{    
    return albedo.Sample(albedo_sampler, data.uv);
}