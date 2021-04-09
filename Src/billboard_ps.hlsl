cbuffer LightBuffer : register(b0)
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightPosition;
    float padding;
};

struct GeoOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 CamPosW: POSITION1;
    float3 NormalW : NORMAL;
    float2 Tex : TEXCOORD;
    uint PrimID : SV_PrimitiveID;
};

float4 main(GeoOut input) : SV_Target
{
    float4	textureColor;
    float3	lightDir;
    float	lightIntensity;
    float4	color;

    // Invert the light direction for calculations.
    lightDir = normalize(float3(input.PosH.xyz) - lightPosition);

    // Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(input.NormalW, -lightDir));

    // Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    color = ambientColor + (diffuseColor * lightIntensity); //adding ambient
    color = saturate(color);


    if (input.PosH.y < -0.6f)
    {
        textureColor = float4(0.93f, 0.79f, 0.7f, 1);
    }
    else if (input.PosH.y < 0.0f)
    {
        textureColor = float4(0.133f, 0.545f, 0.133f, 1);
    }
    else if (input.PosH.y < 0.6f)
    {
        textureColor = float4(0.439f, 0.502f, 0.565f, 1);
    }
    else
    {
        textureColor = float4(1.0f, 0.98f, 0.98f, 1);
    }

    color = color * textureColor;

    return color;
}