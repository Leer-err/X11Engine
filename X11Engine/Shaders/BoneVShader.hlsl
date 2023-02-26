cbuffer windowData : register(b0)
{
    float4x4 projection;
}

cbuffer frameData : register(b1)
{
    float4x4 view;
};

cbuffer modelData : register(b2)
{
    float4x4 world;
    float4x4 normalMatrix;
};

StructuredBuffer<matrix> boneMatrices : register(t0);

struct output
{
    float4 pos : SV_POSITION;
    float3 fragPos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct input
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    int boneIds[4] : BONES;
    float weights[4] : WEIGHTS;
};

output main(in input in_data)
{
    output a;

    a.pos = float4(0.f, 0.f, 0.f, 1.f);

    [unroll(4)]
    for(int i = 0 ; i < 4; i++)
   {
        if(in_data.boneIds[i] == -1) 
            continue;

        matrix boneMatrix = boneMatrices[in_data.boneIds[i]];
        float4 localPosition = mul(float4(in_data.pos,1.0f), boneMatrix);
        float weight = in_data.weights[i];
        
        a.pos.xyz += weight * localPosition.xyz;
    }

    float4 normal = float4(in_data.normal, 1.f);

    a.pos = mul(a.pos, world);
    a.fragPos = a.pos;
    a.pos = mul(a.pos, view);
    a.pos = mul(a.pos, projection);
    a.uv = in_data.uv;
    a.normal = mul(normal, normalMatrix).xyz;
    return a;
}