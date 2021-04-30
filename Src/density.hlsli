#include "sampleNoise.hlsli" 

#define NUM_OCTAVES 5

//float fbm(float3 x) {
//    float v = 0.0;
//    float a = 0.5;
//    float3 shift = float3(1,0,0);
//    for (int i = 0; i < NUM_OCTAVES; ++i) {
//        v += a * snoise(x).x;
//        x = x * 2.0 + shift;
//        a *= 0.5;
//    }
//    return v;
//}

float smooth_snap(float t, float m)
{
    // input: t in [0..1]
    // maps input to an output that goes from 0..1,
    // but spends most of its time at 0 or 1, except for
    // a quick, smooth jump from 0 to 1 around input values of 0.5.
    // the slope of the jump is roughly determined by 'm'.
    // note: 'm' shouldn't go over ~16 or so (precision breaks down).

    //float t1 =     pow((  t)*2, m)*0.5;
    //float t2 = 1 - pow((1-t)*2, m)*0.5;
    //return (t > 0.5) ? t2 : t1;

    // optimized:
    float c = (t > 0.5) ? 1 : 0;
    float s = 1 - c * 2;
    return c + s * pow((c + s * t) * 2, m)*0.5;
}

float DENSITY(float3 ws)
{
   //return ws.z - 5;
    //-----------------------------------------------
    // This function determines the shape of the entire terrain.
    //-----------------------------------------------

    // Remember the original world-space coordinate, 
    // in case we want to use the un-prewarped coord.
    // (extreme pre-warp can introduce small error or jitter to
    //  ws, which, when magnified, looks bad - so in those
    //  cases it's better to use ws_orig.)
    float3 ws_orig = ws;

    // start our density value at zero.
    // think of the density value as the depth beneath the surface 
    // of the terrain; positive values are inside the terrain, and 
    // negative values are in open air.
    float density = 0;

    // sample an ultra-ultra-low-frequency (slowly-varying) float4 
    // noise value we can use to vary high-level terrain features 
    // over space.
    float4 uulf_rand2 = NMQu(ws*0.000632);
    float4 uulf_rand3 = NMQu(ws*0.000695);


    if (ws.y < -30)
    {
        return -1;
    }

    //-----------------------------------------------
    // PRE-WARP the world-space coordinate.
    const float prewarp_str = 15;   // recommended range: 5..25
    float3 ulf_rand = 0;

    // medium-quality version; precision breaks down when pre-warp is strong.
    ulf_rand = NMQs(ws*0.0041).xyz*0.64
        + NMQs(ws*0.0041*0.427).xyz*0.32;

    ws += ulf_rand.xyz * prewarp_str * saturate(uulf_rand3.x*1.4 - 0.3);

    // very general ground plane:
    density = ws.y;
    float hard_floor_y = 0; 
    density += saturate((hard_floor_y - ws_orig.y) * 3) * 40;

    // repeating ridges on [warped] Y coord:
    density += NLQs(ws.xyz*float3(2, 27, 2)*0.0037).x * 2 * saturate(uulf_rand2.w * 2 - 1);

    //
    density +=
        (
            NLQs(ws*0.0200*1.020).x*2.56*1.04 +
            NLQs(ws*0.0100*0.968).x * 5 +
            NMQs(ws*0.0050*0.994).x * 10 * 1.0
        );

    // LOD DENSITY BIAS:
    // this shrinks the lo- and medium-res chunks just a bit,
    // so that the hi-res chunks always "enclose" them:
    // (helps avoid LOD overdraw artifacts)
    float wsChunkSize = 2.0f; // 1, 2, or 4;
    density -= wsChunkSize.x*0.009;

    return density;
}