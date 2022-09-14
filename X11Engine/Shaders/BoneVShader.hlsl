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

tbuffer bonesMatrices : register(t0){
    float4x4 boneMatrices[256];
}

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
    int4 boneIds : BONES;
    float4 weights : WEIGHTS;
};

output main(in input in_data)
{
    output a;

    [unroll(4)]
    for(int i = 0 ; i < 4; i++)
    {
        if(in_data.boneIds[i] == -1) 
            continue;

        float4 localPosition = mul(boneMatrices[in_data.boneIds[i]], float4(in_data.pos,1.0f));
        a.pos += localPosition * in_data.weights[i];
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