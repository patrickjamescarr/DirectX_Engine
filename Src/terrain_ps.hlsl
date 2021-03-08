// Light pixel shader
// Calculate diffuse lighting for a single directional light(also texturing)

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
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 position3D : TEXCOORD2;
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

    if (input.position3D.y < -0.6f)
    {
        textureColor = float4(0.93f, 0.79f, 0.7f, 1);
    }
    else if(input.position3D.y < 0.0f)
    {
        textureColor = float4(0.133f, 0.545f, 0.133f, 1);
    }
    else if (input.position3D.y < 0.6f)
    {
        textureColor = float4(0.439f, 0.502f, 0.565f, 1);
    }
    else
    {
        textureColor = float4(1.0f, 0.98f, 0.98f, 1);
    }

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	//textureColor = shaderTexture.Sample(SampleType, input.tex);
    //textureColor = float4(0, 1, 0, 1);
	color = color * textureColor;

    return color;
}

