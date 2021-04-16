#include "simplex_3d.hlsli" 

float DENSITY(float3 ws)
{
    float density = ws.z;

    return snoise(ws * 0.03f) * 10.0f; // ws.x;//
}