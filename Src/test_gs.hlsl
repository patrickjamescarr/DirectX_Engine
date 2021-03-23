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
	triangle VSOutput input[3],
	inout TriangleStream< GSOutput > output
)
{
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		element.position = input[i].position;
        element.tex = input[i].tex;
        element.normal = input[i].normal;
        element.position3D = input[i].position3D;
		output.Append(element);
	}
}