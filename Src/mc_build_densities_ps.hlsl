// marching cubes pixel shader
#include "density.hlsli"

cbuffer LightBuffer : register(b0)
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightPosition;
    float padding;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float3 wsPosition : TEXCOORD0;
};

float main(PSInput input) : SV_TARGET0
{
    float ret = DENSITY(input.position.xyz / 65.0f);

    return ret;
}

