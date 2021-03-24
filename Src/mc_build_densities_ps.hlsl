// marching cubes pixel shader
#include "density.hlsli"

struct GSOutput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 position3D : TEXCOORD2;
};

float main(GSOutput input) : SV_TARGET0
{
    float ret = DENSITY(input.position3D);

    return ret;
}

