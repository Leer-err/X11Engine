cbuffer frame : register(b0)
{
    float4 ambientColor;
    float3 lightPos;
    float3 viewPos;
}

struct input
{
    float4 pos : SV_POSITION;
    float3 fragPos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

Texture2D tex : register(t0);
SamplerState samp : register(s0);

float4 main(input in_data) : SV_TARGET
{
    float3 lightDir = normalize(lightPos - in_data.fragPos);
    float3 viewDir = normalize(viewPos - in_data.fragPos);
    float3 reflectLight = reflect(-lightDir, in_data.normal);
    
    float4 diffuse = max(dot(in_data.normal, lightDir), 0.f) * float4(.7f, .7f, .7f, .7f);
    float4 specular = pow(max(dot(viewDir, reflectLight), 0.f), 32) * float4(1.f, 1.f, 1.f, 1.f);
    return tex.Sample(samp, in_data.uv) * (ambientColor + diffuse + specular);
}