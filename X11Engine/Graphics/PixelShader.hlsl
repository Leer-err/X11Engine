struct input
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

float4 main(input in_data) : SV_TARGET
{
    return float4(1.f, 1.f, 1.f, 1.f);
}