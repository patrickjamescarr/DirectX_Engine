// basic pixel shader

struct InputType
{
    float4 position : SV_POSITION;
    float3 position3D : TEXCOORD2;
};

float4 main(InputType input) : SV_TARGET
{
    return float4(255.f, 255.f, 255.f, 1.f);
}

