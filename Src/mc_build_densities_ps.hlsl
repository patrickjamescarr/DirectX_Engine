// marching cubes pixel shader
#include "density.hlsli"

struct PSInput
{
    //float4 position     : SV_POSITION;
    float4 wsPosition   : TEXCOORD;
    //float2 tex          : TEXCOORD1;
};

float main(PSInput input) : SV_TARGET
{
    float ret = DENSITY(input.wsPosition.xyz);
    return ret;
}

