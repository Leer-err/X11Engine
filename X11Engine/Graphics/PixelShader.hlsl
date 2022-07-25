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

    float3 ambient;
    float3 diffuse;
    float3 specular;
    
    float constant;
    float lin;
    float quadratic;
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
    float3 viewPos;
    DirLight dirLight;
    PointLight pointLight;
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
    float attentuation = 1.f / (light.constant + light.lin * distance + light.quadratic * pow(distance, 2));

    float3 ambient = light.ambient * diffuseTex.Sample(samp, texCoord).xyz;
    float3 diffuse = light.diffuse * diff * diffuseTex.Sample(samp, texCoord).xyz;
    float3 specular = light.specular * spec * specularTex.Sample(samp, texCoord).xyz;
    
    return (ambient + diffuse + specular) * attentuation;
}

float4 main(input in_data) : SV_TARGET
{
    float3 viewDir = normalize(viewPos - in_data.fragPos);
    
    float4 emission = emissionTex.Sample(samp, in_data.uv);
    
    return float4(CalcDirLight(dirLight, in_data.normal, viewDir, in_data.uv) + CalcPointLight(pointLight, in_data.fragPos, in_data.normal, viewDir, in_data.uv), 1.f) + emission;
}