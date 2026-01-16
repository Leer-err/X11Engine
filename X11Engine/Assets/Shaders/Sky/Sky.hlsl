cbuffer CameraData : register(b0)
{
    matrix view_projection;
    matrix inverse_view_projection;
};

cbuffer SkyData : register(b1)
{
    float height;
    float cloud_plane_scale;
}

struct Vertex_Output{
    float4 viewport_position : SV_POSITION;
    float2 uv: TEXCOORD;
};

struct Vertex_input{
    float3 position : POSITION;
    float2 uv: TEXCOORD;
};

Vertex_Output vertex_main(in Vertex_input vertex)
{
    Vertex_Output output;

    float4 vertex_position = float4(vertex.position, 1);
    vertex_position.xz = vertex_position.xz * cloud_plane_scale;
    vertex_position.y = height;
    output.viewport_position = mul(view_projection, vertex_position);
    output.uv = vertex.uv;

    return output;
}

Texture2D clouds : register(t0);
SamplerState clouds_sampler : register(s0);

float4 pixel_main(in Vertex_Output data) : SV_TARGET
{    
    return clouds.Sample(clouds_sampler, data.uv);
}