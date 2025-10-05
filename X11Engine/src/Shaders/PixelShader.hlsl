struct Input{
    float3 position : POSITION;
    float4 pixel_position: SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D albedo : register(t0);

SamplerState default_sampler : register(s0);

float4 main(in Input data) : SV_TARGET
{    
    return albedo.Sample(default_sampler, data.uv);
}