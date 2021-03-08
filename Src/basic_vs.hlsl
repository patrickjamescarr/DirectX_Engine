// Standard issue vertex shader, apply matrices, pass info to pixel shader

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct InputType
{
    float4 position : POSITION;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD2;
};

OutputType main(InputType input)
{
    OutputType output;

    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // world position of vertex (for point light)
    output.worldPosition = (float3)mul(input.position, worldMatrix);

    return output;
}