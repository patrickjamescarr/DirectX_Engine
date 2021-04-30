#pragma once
#include "DensityFunction.h"
#include "BrownianFunction.h"
#include "Simplex3D.h"

class TerrainDensityFunction :
    public DensityFunction
{
public:
    TerrainDensityFunction();

    double execute(double x, double y, double z) override;
private:
    float clamp(float n, float lower, float upper) {
        return std::max(lower, std::min(n, upper));
    }

    float fbm(DirectX::SimpleMath::Vector3 x);

    DirectX::SimpleMath::Vector4 NLQu(DirectX::SimpleMath::Vector3 uvw);

    DirectX::SimpleMath::Vector4 NLQs(DirectX::SimpleMath::Vector3 uvw);
    DirectX::SimpleMath::Vector4 NMQu(DirectX::SimpleMath::Vector3 uvw);

    DirectX::SimpleMath::Vector4 NMQs(DirectX::SimpleMath::Vector3 uvw);


    float NHQu(DirectX::SimpleMath::Vector3 uvw, float smooth = 1);

    float NHQs(DirectX::SimpleMath::Vector3 uvw, float smooth = 1);

    float lerp(float a, float b, float w)
    {
        return a + w * (b - a);
    }

    DirectX::SimpleMath::Vector2 lerp(DirectX::SimpleMath::Vector2 a, DirectX::SimpleMath::Vector2 b, float w)
    {
        return a + w * (b - a);
    }

    DirectX::SimpleMath::Vector3 lerp(DirectX::SimpleMath::Vector3 a, DirectX::SimpleMath::Vector3 b, float w)
    {
        return a + w * (b - a);
    }

    DirectX::SimpleMath::Vector4 lerp(DirectX::SimpleMath::Vector4 a, DirectX::SimpleMath::Vector4 b, float w)
    {
        return a + w * (b - a);
    }

    float saturate(float val) {
        return std::min(std::max(val, 0.0f),1.0f);
    }

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

private:
    SimplexNoise m_simplex;
private:
    float m_frequency = 1.01f;
    float m_amplitude = 1.f;
    float m_gain = 0.487f;
    int m_octaves = 4;
    Simplex3D m_simplex3D;


};

