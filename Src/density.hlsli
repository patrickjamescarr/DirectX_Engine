#include "simplex_noise.hlsli" 

float DENSITY(float3 ws)
{
    float density = -ws.y;

    density += GenerateSimplex(ws * 0.03f) * 10.0f;

    return density;
}