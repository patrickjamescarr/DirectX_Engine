// Colour pixel/fragment shader
// Basic fragment shader outputting a colour
Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex      : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    // invert the texture v coordinates
    //input.tex.y *= -1;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    float4	color = shaderTexture.Sample(SampleType, input.tex);

    return color;
}