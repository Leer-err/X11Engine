cbuffer frame : register(b0)
{
    float3 ambientColor;
    float3 diffuseColor;
    float3 specularColor;
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

Texture2D baseColor : register(t0);
Texture2D diffuseTex : register(t1);
Texture2D specularTex : register(t2);
SamplerState samp : register(s0);

float4 main(input in_data) : SV_TARGET
{
    float3 lightDir = normalize(lightPos - in_data.fragPos);
    float3 viewDir = normalize(viewPos - in_data.fragPos);
    float3 reflectLight = reflect(-lightDir, in_data.normal);
    
    float3 diffuse = max(dot(in_data.normal, lightDir), 0.f) * diffuseColor;
    float4 diff = float4(diffuse, 1.f) * diffuseTex.Sample(samp, in_data.uv);
    float3 specular = pow(max(dot(viewDir, reflectLight), 0.f), 32) * specularColor;
    float4 spec = float4(specular, 1.f) * specularTex.Sample(samp, in_data.uv);
    
    return baseColor.Sample(samp, in_data.uv) + spec + diff;
}