// colour vertex shader
// Simple geometry pass


cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex      : TEXCOORD0;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex      : TEXCOORD0;
};

OutputType main(InputType input)
{
    OutputType output;

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);


    output.tex = input.tex;

    return output;
}