// colour vertex shader
// Simple geometry pass


cbuffer CameraBuffer : register(b0)
{
    float3 camPosition;
};

struct InputType
{
    float4 position : SV_POSITION;
};

struct OutputType
{
    float4 position : SV_POSITION;
};

OutputType main(InputType input)
{
    OutputType output;

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // pass the camera position through to the geometry shader
    float4 pos = float4(camPosition.xyz, 1.0f);

    output.position = pos;

    return output;
}