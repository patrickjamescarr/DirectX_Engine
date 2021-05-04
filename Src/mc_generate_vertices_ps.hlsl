// marching cubes pixel shader
// Calculate diffuse lighting for a single directional light(also texturing)

cbuffer LightBuffer : register(b0)
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightPosition;
    float padding;
};

struct InputType
{
    float4 wsCoord  : SV_POSITION;
    float3 wsNormal     : NORMAL;
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
    lightDir = normalize(float3(input.position3D.xyz) - lightPosition);

    // Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(input.wsNormal, -lightDir));

    // Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    color = ambientColor + (diffuseColor * lightIntensity); //adding ambient
    color = saturate(color);

    // gray colour for the rock
    textureColor = float4(0.608f, 0.651f, 0.694f, 1.0f);

    color = color * textureColor;

    // Set the color of the fog to dark blue.
    float4 fogColor = float4(0.0f, 0.412f, 0.58f, 1.0f);

    color = input.fogFactor * color + (1.0 - input.fogFactor) * fogColor;

    return color;
}

