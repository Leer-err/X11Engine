<<<<<<< HEAD
#include "ShaderDefines.h"

=======
>>>>>>> 8639105361384194a33ded6c06dbc7c7f29b56e1
struct DirLight
{
    float3 direction;
    
    float3 ambient;
    float3 diffuse;
    float3 specular;
};

struct PointLight
{
    float3 position;
    
    float constant;
    float lin;
    float quadratic;

    float3 ambient;
    float3 diffuse;
    float3 specular;
};

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
Texture2D emissionTex : register(t3);
SamplerState samp : register(s0);

cbuffer frame : register(b0)
{
    float3 ambientColor;
    float3 diffuseColor;
    float3 specularColor;
    float3 lightPos;
    float3 viewPos;
}

float3 CalcDirLight(DirLight light, float3 normal, float3 viewDir, float2 texCoord)
{
    float3 lightDir = -light.direction;
    float3 reflectDir = reflect(-lightDir, normal);
    
    float diff = max(dot(normal, lightDir), 0.f);
    float spec = pow(max(dot(reflectDir, viewDir), 0.f), 32);

    float3 ambient = light.ambient * diffuseTex.Sample(samp, texCoord).xyz;
    float3 diffuse = light.diffuse * diff * diffuseTex.Sample(samp, texCoord).xyz;
    float3 specular = light.specular * spec * specularTex.Sample(samp, texCoord).xyz;
    
    return ambient + diffuse + specular;
}

float3 CalcPointLight(PointLight light, float3 pos, float3 normal, float3 viewDir, float2 texCoord)
{
    float3 lightDir = normalize(light.position - pos);
    float3 reflectDir = reflect(-lightDir, normal);
    
    float diff = max(dot(normal, lightDir), 0.f);
    float spec = pow(max(dot(reflectDir, viewDir), 0.f), 32);
    
    float distance = length(light.position - pos);
    float attentuation = 1.f / (light.constant + light.lin * distance + light.quadratic + pow(distance, 2));

    float3 ambient = light.ambient * diffuseTex.Sample(samp, texCoord).xyz;
    float3 diffuse = light.diffuse * diff * diffuseTex.Sample(samp, texCoord).xyz;
    float3 specular = light.specular * spec * specularTex.Sample(samp, texCoord).xyz;
    
    return (ambient + diffuse + specular) * attentuation;
}

float4 main(input in_data) : SV_TARGET
{
    float3 lightDir = normalize(lightPos - in_data.fragPos);
    float3 viewDir = normalize(viewPos - in_data.fragPos);
    float3 reflectLight = reflect(-lightDir, in_data.normal);
    
    float3 diffuse = max(dot(in_data.normal, lightDir), 0.f) * diffuseColor;
    float4 diff = float4(diffuse, 1.f) * diffuseTex.Sample(samp, in_data.uv);
    float3 specular = pow(max(dot(viewDir, reflectLight), 0.f), 32) * specularColor;
    float4 spec = float4(specular, 1.f) * specularTex.Sample(samp, in_data.uv);
    
    float4 emission = emissionTex.Sample(samp, in_data.uv);
    
    return float4(ambientColor, 1.f) * diffuseTex.Sample(samp, in_data.uv) + spec + diff + emission;
}