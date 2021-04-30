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

    //// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    //float4 pos = (float4(camPosition.xyz, 1.0f) / 0.12) + 1;

    //pos.x -= 16.50f;
    //pos.y -= 16.50f;

    float4 pos = float4(camPosition.xyz, 1.0f);

    output.position = pos;


    return output;
}