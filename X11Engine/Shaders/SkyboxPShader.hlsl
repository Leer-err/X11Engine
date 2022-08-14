struct VSout{
    float4 resPosition: SV_Position;
    float3 pos: POSITION;
};

TextureCube texCube : register(t0);
SamplerState samplerState : register(s0);

float4 main(VSout input) : SV_Target{
    return texCube.Sample(samplerState, normalize(input.pos));
}