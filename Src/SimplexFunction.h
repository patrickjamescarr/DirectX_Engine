#pragma once
#include "DensityFunction.h"
#include "SimplexNoise.h"

class SimplexFunction : public DensityFunction
{
friend class MarchingCubesMesh;
public:
    double execute(double x, double y, double z) override { return m_simplex.Generate(DirectX::SimpleMath::Vector3(x, y, z)) * m_multiplier; }
private:
    SimplexNoise m_simplex;
    const float m_multiplier = 10;
};

