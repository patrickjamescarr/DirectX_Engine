#pragma once
#include "DensityFunction.h"
#include "fBm.h"

class BrownianFunction :
    public DensityFunction
{
    friend class MarchingCubesMesh;
public:
    double execute(double x, double y, double z) override { return m_fbm.Generate(DirectX::SimpleMath::Vector3(x, y, z), m_value, m_amplitude, m_octaves, m_lacunarity, m_gain); }
private:
    FBM m_fbm;

    float m_amplitude = 0.5f;
    float m_value = 0.0f;
    int m_octaves = 6;
    float m_lacunarity = 2.0f;
    float m_gain = 0.5f;


};

