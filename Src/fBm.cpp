#include "pch.h"
#include "FBM.h"

using namespace DirectX::SimpleMath;

FBM::FBM()
{
}

float FBM::Generate(DirectX::SimpleMath::Vector2 point, float value, float amplitude, int octaves, float lacunarity, float gain)
{
    // Loop of octaves
    for (int i = 0; i < octaves; i++) {
        value += amplitude * noise(point);
        point *= lacunarity;
        amplitude *= gain;
    }
    return value;
}

float FBM::random(Vector2 point)
{
    auto val = fract(sin(point.Dot(Vector2(12.9898, 78.233))) * 43758.5453123);

    return val;
}

float FBM::noise(Vector2 point)
{
    Vector2 i = Vector2(floor(point.x), floor(point.y));
    Vector2 f = point - i;

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + Vector2(1.0, 0.0));
    float c = random(i + Vector2(0.0, 1.0));
    float d = random(i + Vector2(1.0, 1.0));

    Vector2 u = f * f * ((3.0f - 2.0f) * f);

    return mix(a, b, u.x) +
        (c - a)* u.y * (1.0 - u.x) +
        (d - b) * u.x * u.y;
}

