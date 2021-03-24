#include "sampleNoise.hlsli" 

float DENSITY(float3 ws)
{
    float density = -ws.y;

    density += noiseVol.Sample(TrilinearRepeat, ws).x;
}