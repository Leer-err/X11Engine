struct Vertex_Output{
    float4 viewport_position : SV_POSITION;
    float2 uv : TEXCOORD;
};

struct Vertex_input{
    float3 position : POSITION;
};

Vertex_Output vertex_main(in Vertex_input vertex)
{
    Vertex_Output output;
    output.viewport_position = float4(vertex.position, 1);
    output.uv = vertex.position.xy;

    return output;
}

float3 hash( float3 p ) 
{
	p = float3( dot(p,float3(127.1,311.7, 74.7)),
			  dot(p,float3(269.5,183.3,246.1)),
			  dot(p,float3(113.5,271.9,124.6)));

	return -1.0 + 2.0*frac(sin(p)*43758.5453123);
}

float noise( in float3 x )
{
    // grid
    float3 i = floor(x);
    float3 f = frac(x);
    
    // quintic interpolant
    float3 u = f*f*f*(f*(f*6.0-15.0)+10.0);
    
    // gradients
    float3 ga = hash( i+float3(0.0,0.0,0.0) );
    float3 gb = hash( i+float3(1.0,0.0,0.0) );
    float3 gc = hash( i+float3(0.0,1.0,0.0) );
    float3 gd = hash( i+float3(1.0,1.0,0.0) );
    float3 ge = hash( i+float3(0.0,0.0,1.0) );
    float3 gf = hash( i+float3(1.0,0.0,1.0) );
    float3 gg = hash( i+float3(0.0,1.0,1.0) );
    float3 gh = hash( i+float3(1.0,1.0,1.0) );
    
    // projections
    float va = dot( ga, f-float3(0.0,0.0,0.0) );
    float vb = dot( gb, f-float3(1.0,0.0,0.0) );
    float vc = dot( gc, f-float3(0.0,1.0,0.0) );
    float vd = dot( gd, f-float3(1.0,1.0,0.0) );
    float ve = dot( ge, f-float3(0.0,0.0,1.0) );
    float vf = dot( gf, f-float3(1.0,0.0,1.0) );
    float vg = dot( gg, f-float3(0.0,1.0,1.0) );
    float vh = dot( gh, f-float3(1.0,1.0,1.0) );
	
    // interpolation
    return va + 
           u.x*(vb-va) + 
           u.y*(vc-va) + 
           u.z*(ve-va) + 
           u.x*u.y*(va-vb-vc+vd) + 
           u.y*u.z*(va-vc-ve+vg) + 
           u.z*u.x*(va-vb-ve+vf) + 
           u.x*u.y*u.z*(-va+vb+vc-vd+ve-vf-vg+vh);
}

float fbm( in float3 x, in float H)
{    
    const int numOctaves = 4;

    float G = exp2(-H);
    float f = 1.0;
    float a = 1.0;
    float t = 0.0;
    for( int i=0; i<numOctaves; i++ )
    {
        t += a*noise(f*x);
        f *= 2.0;
        a *= G;
    }
    return t;
}

cbuffer CloudData : register(b0)
{
    float3 cloud_color;
    float time;
};

float4 pixel_main(in Vertex_Output data) : SV_TARGET
{
    const float radius = 0.5;
    float distance = radius - length(data.uv) + 0.5;
    distance = clamp(distance, 0, 1);

    const float scale = 5; 
    float2 cloud_coords = data.uv * scale + time * 0.1;
    float clouds = fbm(float3(cloud_coords, 0), 0.5);

    float cloud_strength = distance * clouds;

    return float4(cloud_strength * cloud_color, cloud_strength);
}