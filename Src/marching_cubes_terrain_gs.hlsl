struct GSOutput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 position3D : TEXCOORD2;
};

struct VSOutput
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 position3D : TEXCOORD2;
};

[maxvertexcount(3)]
void main(
    point VSOutput input[1],
    inout TriangleStream< GSOutput > output
)
{
    GSOutput element;
    element.position = input[0].position;
    element.tex = input[0].tex;
    element.normal = input[0].normal;
    element.position3D = input[0].position3D;

    output.Append(element);

    float4 p2 = float4(input[0].position.x + 3, input[0].position.y + 3, input[0].position.z, input[0].position.w);
    float4 p3 = float4(input[0].position.x + 6, input[0].position.y, input[0].position.z, input[0].position.w);

    element.position = p2;
    element.tex = input[0].tex;
    element.normal = input[0].normal;
    element.position3D = input[0].position3D;

    output.Append(element);

    element.position = p3;
    element.tex = input[0].tex;
    element.normal = input[0].normal;
    element.position3D = input[0].position3D;

    output.Append(element);

}