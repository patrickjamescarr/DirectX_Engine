// fog vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader
// calculate fog factor

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

// Fog buffer
cbuffer FogBuffer: register(b1)
{
    float4 fogEnd;
    float4 cameraPosition;
};

struct InputType
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD0;
};

struct OutputType
{
    float4 position     : SV_POSITION;
    float2 tex          : TEXCOORD0;
    float3 normal       : NORMAL;
    float3 position3D   : TEXCOORD2;
    float  fogFactor    : FOG;
};

// calculate linear fog
float CalculateLinearFogFactor(float3 vertexPosition, float3 camPosition)
{
    float fogFactor;

    float camDistanceToVertex = length(vertexPosition - camPosition);

    if (camDistanceToVertex >= fogEnd.x)
    {
        return 0;
    }

    fogFactor = saturate((fogEnd.x - camDistanceToVertex) / fogEnd.x);

    return fogFactor;
}

OutputType main(InputType input)
{
    OutputType output;

    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

    // Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);

    // Normalize the normal vector.
    output.normal = normalize(output.normal);

    // world position of vertex (for point light)
    output.position3D = (float3)mul(input.position, worldMatrix);

    // calculate fog factor
    float4 camWS;

    output.fogFactor = CalculateLinearFogFactor(output.position3D, cameraPosition.xyz);

    return output;
}