// marching cubes build densities vertex shader

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VSInput
{
    float4 position : POSITION;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 wsPosition : TEXCOORD0;
};

VSOutput main(VSInput input)
{
    VSOutput output;

    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // world position of vertex
    output.wsPosition = (float3)mul(input.position, worldMatrix);

    return output;
}