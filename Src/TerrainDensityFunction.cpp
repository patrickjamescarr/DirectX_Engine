#include "pch.h"
#include "TerrainDensityFunction.h"
#include <math.h>
#include <algorithm>

using namespace DirectX::SimpleMath;

TerrainDensityFunction::TerrainDensityFunction(BrownianFunction * fbm)
    : m_fbm(fbm)
{
}

double TerrainDensityFunction::execute(double x, double y, double z)
{
    float density = -y;

    float hard_floor_y = 0.05; 
    
    density += clamp(((hard_floor_y - y) * 3) * 40, 0.f, 1.f);

    //density += m_simplex.Generate(x*1.01, y*1.01, z*1.01)*1.00;

    //density += m_simplex.Generate(x*1.96, y*1.96, z*1.96)*0.50;

    //density += m_simplex.Generate(x*4.03, y*4.03, z*4.03)*0.25;

    float frequency = m_frequency;
    float amplitude = m_amplitude;

    for (int i = 0; i < m_octaves; i++)
    {
        auto point = DirectX::SimpleMath::Vector3(x, y, z);
        density += m_simplex.Generate(point * frequency)*amplitude;
        amplitude *= 0.5;
        frequency *= 1.945;
    }

    return density;
}

float TerrainDensityFunction::noise(Vector3 x)
{
    {
        // The noise function returns a value in the range -1.0f -> 1.0f

        double intpart;

        Vector3 p = Vector3(floor(x.x), floor(x.y), floor(x.z));
        Vector3 f = Vector3(modf(x.x, &intpart), modf(x.y, &intpart), modf(x.z, &intpart));


        //f = f * f*(3.0 - 2.0*f);
        //float n = p.x + p.y*57.0 + 113.0*p.z;

        //return lerp(lerp(lerp(hash(n + 0.0), hash(n + 1.0), f.x),
        //    lerp(hash(n + 57.0), hash(n + 58.0), f.x), f.y),
        //    lerp(lerp(hash(n + 113.0), hash(n + 114.0), f.x),
        //        lerp(hash(n + 170.0), hash(n + 171.0), f.x), f.y), f.z);

        return 0.0f;
    }
}
