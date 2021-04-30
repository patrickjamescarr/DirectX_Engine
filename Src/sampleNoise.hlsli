#include "simplex_3d.hlsli" 

#define NOISE_LATTICE_SIZE 16
#define INV_LATTICE_SIZE (1.0/(float)(NOISE_LATTICE_SIZE))

#define NUM_OCTAVES 5

float fbm(float3 x) {

    float v = 0.0;
    float a = 0.5;
    float3 shift = float3(1, 0, 0);
    for (int i = 0; i < NUM_OCTAVES; ++i) {
        v += a * snoise(x);
        x = x * 2.0 + shift;
        a *= 0.5;
    }
    return v;
}

float4 NLQu(float3 uvw) {

    float4 noiseVal;

    float amp = 5.0f;

    noiseVal.x = fbm(uvw) * amp;
    noiseVal.y = fbm(uvw + float3(1, 0, 0))* amp;
    noiseVal.z = fbm(uvw + float3(0, 1, 0))* amp;
    noiseVal.w = fbm(uvw + float3(0, 0, 1))* amp;

    return noiseVal;
}

float4 NLQs(float3 uvw) {
    return NLQu(uvw) * 2 - 1;
}

float4 NMQu(float3 uvw) {
    // smooth the input coord
    float3 t = frac(uvw * NOISE_LATTICE_SIZE + 0.5);
    float3 t2 = (3 - 2 * t)*t*t;
    float3 uvw2 = uvw + (t2 - t) / (float)(NOISE_LATTICE_SIZE);
    // fetch
    return NLQu(uvw2);
}

float4 NMQs(float3 uvw) {
    // smooth the input coord
    float3 t = frac(uvw * NOISE_LATTICE_SIZE + 0.5);
    float3 t2 = (3 - 2 * t)*t*t;
    float3 uvw2 = uvw + (t2 - t) / (float)(NOISE_LATTICE_SIZE);
    // fetch  
    return NLQs(uvw2);
}

// SUPER MEGA HIGH QUALITY noise sampling (signed)
float NHQu(float3 uvw, float smooth = 1)
{
    float amp = 5.0f;
    float3 uvw2 = floor(uvw * NOISE_LATTICE_SIZE) * INV_LATTICE_SIZE;
    float3 t = (uvw - uvw2) * NOISE_LATTICE_SIZE;
    t = lerp(t, t*t*(3 - 2 * t), smooth);

    float2 d = float2(INV_LATTICE_SIZE, 0);

#if 0
    // the 8-lookup version... (SLOW)
    float4 f1 = float4(
        fbm(uvw2 + d.xxx)* amp,
        fbm(uvw2 + d.yxx)* amp,
        fbm(uvw2 + d.xyx)* amp,
        fbm(uvw2 + d.yyx)* amp;
    float4 f2 = float4(
        fbm(uvw2 + d.xxy)* amp,
        fbm(uvw2 + d.yxy)* amp,
        fbm(uvw2 + d.xyy)* amp,
        fbm(uvw2 + d.yyy, 0).x)* amp;
    float4 f3 = lerp(f2, f1, t.zzzz);
    float2 f4 = lerp(f3.zw, f3.xy, t.yy);
    float  f5 = lerp(f4.y, f4.x, t.x);
#else
    // THE TWO-SAMPLE VERSION: much faster!
    // note: requires that three YZ-neighbor texels' original .x values
    //       are packed into .yzw values of each texel.
    float4 f1 = NLQu(uvw2).zxyw; // <+0, +y, +z, +yz>
    float4 f2 = NLQu(uvw2 + d.xyy).zxyw; // <+x, +xy, +xz, +xyz>
    float4 f3 = lerp(f1, f2, t.xxxx);  // f3 = <+0, +y, +z, +yz>
    float2 f4 = lerp(f3.xy, f3.zw, t.yy); // f4 = <+0, +z>
    float  f5 = lerp(f4.x, f4.y, t.z);
#endif

    return f5;
}

float NHQs(float3 uvw, float smooth = 1) {
    return NHQu(uvw, smooth) * 2 - 1;
}

