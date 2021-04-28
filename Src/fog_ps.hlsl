// fog pixel shader
// Calculate diffuse lighting for a single directional light(also texturing) with fog calculations

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);


cbuffer LightBuffer : register(b0)
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightPosition;
    float padding;
};

struct InputType
{
    float4 position     : SV_POSITION;
    float2 tex          : TEXCOORD0;
    float3 normal       : NORMAL;
    float3 position3D   : TEXCOORD2;
    float  fogFactor    : FOG;
};

float4 main(InputType input) : SV_TARGET
{
    float4	textureColor;
    float3	lightDir;
    float	lightIntensity;
    float4	color;

    // Invert the light direction for calculations.
    lightDir = normalize(input.position3D - lightPosition);

    // Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(input.normal, -lightDir));

    // Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    color = ambientColor + (diffuseColor * lightIntensity); //adding ambient
    color = saturate(color);

    // invert the texture v coordinates
    input.tex.y *= -1;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);
    color = color * textureColor;

    // Set the color of the fog to dark blue.
    float4 fogColor = float4(0.0f, 0.412f, 0.58f, 1.0f);

    color = input.fogFactor * color + (1.0 - input.fogFactor) * fogColor;

    return color;
}

