#include "pch.h"
#include "TerrainDensityFunction.h"

using namespace DirectX::SimpleMath;

TerrainDensityFunction::TerrainDensityFunction()
{
}

double TerrainDensityFunction::execute(double x, double y, double z)
{
    //-----------------------------------------------
    // This function determines the shape of the entire terrain.
    //-----------------------------------------------

    // Remember the original world-space coordinate, 
    // in case we want to use the un-prewarped coord.
    // (extreme pre-warp can introduce small error or jitter to
    //  ws, which, when magnified, looks bad - so in those
    //  cases it's better to use ws_orig.)
    auto ws = Vector3(x, y, z);
    Vector3 ws_orig = Vector3(x, y, z);

    // start our density value at zero.
    // think of the density value as the depth beneath the surface 
    // of the terrain; positive values are inside the terrain, and 
    // negative values are in open air.
    float density = 0;

    // sample an ultra-ultra-low-frequency (slowly-varying) Vector4 
    // noise value we can use to vary high-level terrain features 
    // over space.
    Vector4 uulf_rand2 = NMQu(ws*0.000632);
    Vector4 uulf_rand3 = NMQu(ws*0.000695);


    if (ws.y < -30)
    {
        return -1;
    }

    //-----------------------------------------------
    // PRE-WARP the world-space coordinate.
    const float prewarp_str = 15;   // recommended range: 5..25
    Vector3 ulf_rand = Vector3::Zero;

    // medium-quality version; precision breaks down when pre-warp is strong.
    ulf_rand = (Vector3)NMQs(ws*0.0041)*0.64
        + (Vector3)NMQs(ws*0.0041*0.427)*0.32;

    ws += ulf_rand * prewarp_str * saturate(uulf_rand3.x*1.4 - 0.3);

    // very general ground plane:
    density = ws.y;
    float hard_floor_y = 0;
    density += saturate((hard_floor_y - ws_orig.y) * 3) * 40;

    // repeating ridges on [warped] Y coord:
    density += NLQs(ws*Vector3(2, 27, 2)*0.0037).x * 2 * saturate(uulf_rand2.w * 2 - 1);

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
    density -= wsChunkSize*0.009;

    return density;
}

#define NOISE_LATTICE_SIZE 16
#define INV_LATTICE_SIZE (1.0/(float)(NOISE_LATTICE_SIZE))

#define NUM_OCTAVES 5

using namespace DirectX::SimpleMath;

float TerrainDensityFunction::fbm(Vector3 x)
{
    auto noise = m_simplex3D.snoise(x);
    float v = 0.0;
    float a = 0.5;
    Vector3 shift = Vector3(1, 0, 0);
    for (int i = 0; i < NUM_OCTAVES; ++i) {
        v += a * noise;
        x = x * 2.0 + shift;
        a *= 0.5;
    }
    return v;
}

Vector4 TerrainDensityFunction::NLQu(Vector3 uvw)
{
    Vector4 noiseVal;

    float amp = 5.0f;

    noiseVal.x = fbm(uvw) * amp;
    noiseVal.y = fbm(uvw + Vector3(1, 0, 0))* amp;
    noiseVal.z = fbm(uvw + Vector3(0, 1, 0))* amp;
    noiseVal.w = fbm(uvw + Vector3(0, 0, 1))* amp;

    return noiseVal;
}

Vector4 TerrainDensityFunction::NLQs(Vector3 uvw)
{
    return NLQu(uvw) * 2 - Vector4(1, 1, 1, 1);
}

Vector4 TerrainDensityFunction::NMQu(Vector3 uvw)
{
    float x, y, z;

    // smooth the input coord
    Vector3 temp = uvw * NOISE_LATTICE_SIZE + Vector3(0.5, 0.5, 0.5);
    Vector3 t = Vector3(modf(temp.x, &x), modf(temp.y, &y), modf(temp.z, &z));
    Vector3 t2 = (Vector3(3, 3, 3) - 2 * t)*t*t;
    Vector3 uvw2 = uvw + (t2 - t) / (float)(NOISE_LATTICE_SIZE);
    // fetch
    return NLQu(uvw2);
}

Vector4 TerrainDensityFunction::NMQs(Vector3 uvw)
{
    float x, y, z;

    // smooth the input coord
    Vector3 temp = uvw * NOISE_LATTICE_SIZE + Vector3(0.5, 0.5, 0.5);
    Vector3 t = Vector3(modf(temp.x, &x), modf(temp.y, &y), modf(temp.z, &z));
    Vector3 t2 = (Vector3(3, 3, 3) - 2 * t)*t*t;
    Vector3 uvw2 = uvw + (t2 - t) / (float)(NOISE_LATTICE_SIZE);
    // fetch  
    return NLQs(uvw2);
}

float TerrainDensityFunction::NHQu(Vector3 uvw, float smooth)
{
    float amp = 5.0f;

    auto temp = (uvw * NOISE_LATTICE_SIZE);

    Vector3 uvw2 = Vector3(std::floor(temp.x), std::floor(temp.y), std::floor(temp.z)) * INV_LATTICE_SIZE;
    Vector3 t = (uvw - uvw2) * NOISE_LATTICE_SIZE;

    t = lerp(t, t * t*(Vector3(3, 3, 3) - 2 * t), smooth);
    Vector2 d = Vector2(INV_LATTICE_SIZE, 0);


    // THE TWO-SAMPLE VERSION: much faster!
    // note: requires that three YZ-neighbor texels' original .x values
    //       are packed into .yzw values of each texel.
    Vector4 f1 = NLQu(uvw2); // <+0, +y, +z, +yz>
    f1 = Vector4(f1.z, f1.x, f1.y, f1.w);
    Vector4 f2 = NLQu(uvw2 + Vector3(d.x, d.y, d.y)); // <+x, +xy, +xz, +xyz>
    f2 = Vector4(f2.z, f2.x, f2.y, f2.w);
    Vector4 f3 = lerp(f1, f2, t.x);  // f3 = <+0, +y, +z, +yz>
    Vector2 f4 = lerp(Vector2(f3.x, f3.y), Vector2(f3.z, f3.w), t.y); // f4 = <+0, +z>
    float  f5 = lerp(f4.x, f4.y, t.z);


    return f5;
}

float TerrainDensityFunction::NHQs(Vector3 uvw, float smooth)
{
    return NHQu(uvw, smooth) * 2 - 1;
}

