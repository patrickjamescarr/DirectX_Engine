// Colour pixel/fragment shader
// Basic fragment shader outputting a colour

struct InputType
{
    float4 position     : SV_POSITION;
    float3 worldPosition : TEXCOORD;
};

float4 main(InputType input) : SV_TARGET
{
    return float4(255.f, 255.f, 255.f, 1.f);
}