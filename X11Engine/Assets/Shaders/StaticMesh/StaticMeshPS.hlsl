struct Input{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float4 viewport_position : SV_POSITION;
};

float4 main(in Input data) : SV_TARGET
{    
    return float4(1.0,1.0,1.0,1.0);
}