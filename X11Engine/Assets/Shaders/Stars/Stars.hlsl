cbuffer CameraData : register(b0)
{
    Matrix view_projection;
    Matrix inverse_view_projection;
};

struct Vertex_Output{
    float4 viewport_position : SV_POSITION;
    float3 position : POSITION;
};

struct Vertex_input{
    float3 position : POSITION;
};

Vertex_Output vertex_main(in Vertex_input vertex)
{
    Vertex_Output output;

    output.viewport_position = float4(vertex.position, 1);
    float4 position = mul(inverse_view_projection, output.viewport_position);
    position = position / position.w;
    output.position = position.xyz;

    return output;
}

float3 hash(float3 p) {
  return frac(
      sin(float3(dot(p, float3(1.0, 57.0, 113.0)), dot(p, float3(57.0, 113.0, 1.0)),
               dot(p, float3(113.0, 1.0, 57.0)))) *
      43758.5453);
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
    const int numOctaves = 8;

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

static const float2x2 myt = float2x2(.12121212, .13131313, -.13131313, .12121212);
static const float2 mys = float2(1e4, 1e6);

float2 rhash(float2 uv) {
  uv = mul(myt, uv);
  uv = uv * mys;
  return frac(frac(uv / mys) * uv);
}

float3 voronoi3d(const in float3 x) {
  float3 p = floor(x);
  float3 f = frac(x);

  float id = 0.0;
  float2 res = float2(100, 100);
  for (int k = -1; k <= 1; k++) {
    for (int j = -1; j <= 1; j++) {
      for (int i = -1; i <= 1; i++) {
        float3 b = float3(float(i), float(j), float(k));
        float3 r = float3(b) - f + hash(p + b);
        float d = dot(r, r);

        float cond = max(sign(res.x - d), 0.0);
        float nCond = 1.0 - cond;

        float cond2 = nCond * max(sign(res.y - d), 0.0);
        float nCond2 = 1.0 - cond2;

        id = (dot(p + b, float3(1.0, 57.0, 113.0)) * cond) + (id * nCond);
        res = float2(d, res.x) * cond + res * nCond;

        res.y = cond2 * d + nCond2 * res.y;
      }
    }
  }

  return float3(sqrt(res), abs(id));
}

float remap(float value, float in_min, float in_max, float out_min, float out_max)
{
    float t = (value - in_min) / (in_max - in_min);
    
    return lerp(out_min, out_max, t);
}

cbuffer StarsData : register(b0)
{
    float time;
    float star_density;
    float blinking_speed;
    float blink_strength;
}

float4 pixel_main(in Vertex_Output data) : SV_TARGET
{
    float3 noise_position = normalize(data.position);
    float noise = 1 - voronoi3d(noise_position * star_density);
    float stars = step(0.95, noise);

    float blink_offset = time * blinking_speed;
    float brightness_noise = fbm((noise_position * 10 + blink_offset) , 0.5);
    float star_brightness = remap(brightness_noise, 0, 1, 1 - blink_strength, 1);

    stars = stars * star_brightness;

    return stars;
}