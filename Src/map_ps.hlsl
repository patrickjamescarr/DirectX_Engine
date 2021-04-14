// marching cubes pixel shader
#include "density.hlsli"

struct PSInput
{
    float4 position     : SV_POSITION;
    float4 tex          : TEXCOORD;
    float3 position3D   : TEXCOORD2;
};

float main(PSInput input) : SV_TARGET
{
    float ret = DENSITY(input.position3D.xyz);

    return ret;
}

