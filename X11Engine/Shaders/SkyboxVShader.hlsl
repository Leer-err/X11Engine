cbuffer windowData : register(b0)
{
    float4x4 projection;
}

cbuffer frameData : register(b1)
{
    float4x4 view;
};

struct VSout{
    float4 resPosition: SV_Position;
    float3 pos: POSITION;
};

VSout main(float3 position : POSITION){
    VSout output;
    float3 viewPosition = mul(position, (float3x3)view);
    output.resPosition = mul(float4(viewPosition, 1.f), projection).xyww;
    output.pos = position;
    return output;
}