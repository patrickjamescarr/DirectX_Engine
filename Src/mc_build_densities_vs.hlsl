// marching cubes build densities vertex shader

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VSInput
{
    float4 position     : POSITION;
    float2 tex          : TEXCOORD;
    uint   nInstanceID  : SV_InstanceID;   // generated by the vertex shader. each slice is an instance
};

struct VSOutput
{
    float4 position     : POSITION;
    float4 wsPosition   : TEXCOORD;
    float2 tex          : TEXCOORD1;
    uint   nInstanceID  : SLICEINDEX;
};

VSOutput main(VSInput input)
{
    VSOutput output;

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    output.wsPosition = float4(input.tex.xy, input.nInstanceID * (1.0f / 10.0f), 1.0f);

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

    output.nInstanceID = input.nInstanceID;

    return output;
}