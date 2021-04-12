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
    auto point = DirectX::SimpleMath::Vector3(x, y, z);
    float density = -y;

    //density += m_simplex.Generate(point)*10.f;

    //float hard_floor_y = 0.05; 
    //
    //density += clamp(((hard_floor_y - y) * 3) * 40, 0.f, 1.f);

    ////density += m_simplex.Generate(x*1.01, y*1.01, z*1.01)*1.00;

    ////density += m_simplex.Generate(x*1.96, y*1.96, z*1.96)*0.50;

    ////density += m_simplex.Generate(x*4.03, y*4.03, z*4.03)*0.25;

    //float frequency = m_frequency;
    //float amplitude = m_amplitude;

    //for (int i = 0; i < m_octaves; i++)
    //{
    //    auto point = DirectX::SimpleMath::Vector3(x, y, z);
    //    density += m_simplex.Generate(point * frequency)*amplitude;
    //    amplitude *= 0.5;
    //    frequency *= 1.945;
    //}

    return density;
}

