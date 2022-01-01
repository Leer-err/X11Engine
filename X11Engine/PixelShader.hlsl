struct input
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

float4 main(input color) : SV_TARGET
{
    return color.color;
}