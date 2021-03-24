#include "simplex_noise.hlsli" 

float DENSITY(float3 ws)
{
    float density = -ws.y;

    density += GenerateSimplex(ws);

    return density;
}