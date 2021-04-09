// Texture pixel/fragment shader

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float main(InputType input) : SV_TARGET
{
    // invert the texture v coordinates
    // input.tex.y *= -1;

    float val;

    val = input.tex.x + input.tex.y;

    return val;
}