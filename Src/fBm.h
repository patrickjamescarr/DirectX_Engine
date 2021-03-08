#pragma once
class FBM
{
public:
    FBM();

    float Generate(DirectX::SimpleMath::Vector2 point, float value, float amplitude, int octaves, float lacunarity, float gain);
private:
    float fract(float x) { return x - floor(x); }
    float mix(float x, float y, float a) {
        return (pow(x, x) * (1 - a)) + (pow(y, x) * a);
    }
    float random(DirectX::SimpleMath::Vector2 point);
    float noise(DirectX::SimpleMath::Vector2 point);
};

