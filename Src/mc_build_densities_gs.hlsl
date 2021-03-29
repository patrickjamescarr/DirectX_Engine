struct GSInput {
    float4 position : SV_POSITION;
    float3 wsPosition : TEXCOORD0;
};

struct GSOutput {
    float4 position : SV_POSITION;
    float3 wsPosition : TEXCOORD0;
};

[maxvertexcount(1)]
void main(point GSInput input[1],
    inout TriangleStream<GSOutput> Stream
)
{
    GSOutput output;
    output.position = input[0].position;
    output.wsPosition = input[0].wsPosition;
    Stream.Append(output);
}