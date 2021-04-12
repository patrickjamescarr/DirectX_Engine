#pragma once
#include "DensityFunction.h"
#include "BrownianFunction.h"

class TerrainDensityFunction :
    public DensityFunction
{
public:
    TerrainDensityFunction(BrownianFunction* fbm);

    double execute(double x, double y, double z) override;
private:
    float clamp(float n, float lower, float upper) {
        return std::max(lower, std::min(n, upper));
    }

private:
    BrownianFunction* m_fbm;
    SimplexNoise m_simplex;
private:
    float m_frequency = 1.01f;
    float m_amplitude = 1.f;
    float m_gain = 0.487f;
    int m_octaves = 4;
};

