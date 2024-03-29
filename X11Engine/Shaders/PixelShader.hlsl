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
Texture2D specularTex : register(t1);
Texture2D emissionTex : register(t2);
SamplerState samp : register(s0);

StructuredBuffer<PointLight> pointLights : register(t3);

cbuffer frame : register(b0)
{
    float3 viewPos;
    DirLight dirLight;
}

float3 CalcDirLight(DirLight light, float3 normal, float3 viewDir, float2 texCoord)
{
    float3 lightDir = -light.direction;
    float3 halfway = normalize(lightDir + viewDir);
    
    float4 coeficents = lit(dot(normal,lightDir), dot(normal, halfway), 32);

    float3 ambient = light.ambient * baseColor.Sample(samp, texCoord).xyz;
    float3 diffuse = light.diffuse * coeficents.y * baseColor.Sample(samp, texCoord).xyz;
    float3 specular = light.specular * coeficents.z * specularTex.Sample(samp, texCoord).xyz;
    
    return ambient + diffuse + specular;
}

float3 CalcPointLight(PointLight light, float3 pos, float3 normal, float3 viewDir, float2 texCoord)
{
    float3 lightDir = normalize(light.position - pos);
    float3 halfway = normalize(lightDir + viewDir);

    float4 coeficents = lit(dot(normal,lightDir), dot(normal, halfway), 32);

    float dist = distance(light.position, pos);
    float attentuation = rcp(light.constant + light.lin * dist + light.quadratic * pow(dist, 2));

    float3 ambient = light.ambient * baseColor.Sample(samp, texCoord).xyz;
    float3 diffuse = light.diffuse * coeficents.y * baseColor.Sample(samp, texCoord).xyz;
    float3 specular = light.specular * coeficents.z * specularTex.Sample(samp, texCoord).xyz;
    
    return (ambient + diffuse + specular) * attentuation;
}

float4 main(in input in_data) : SV_TARGET
{
    uint numLights, dump;
    
    float3 viewDir = normalize(viewPos - in_data.fragPos);
    
    float4 emission = emissionTex.Sample(samp, in_data.uv);
    
    float3 result = CalcDirLight(dirLight, in_data.normal, viewDir, in_data.uv);
    
    pointLights.GetDimensions(numLights, dump);
    
    for (int i = 0; i < numLights; i++)
    {
        result += CalcPointLight(pointLights[i], in_data.fragPos, in_data.normal, viewDir, in_data.uv);
    }
    
    return saturate(float4(result, 1.f) + emission);
}