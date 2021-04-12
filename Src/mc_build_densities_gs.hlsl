struct VSOutput {
    float4 position     : POSITION;
    float4 wsPosition   : TEXCOORD0;
    //float2 tex          : TEXCOORD1;
    uint   nInstanceID  : SLICEINDEX;
};

struct GSOutput {
    float4 wsPosition   : TEXCOORD;
    float4 position     : SV_POSITION;

    //float2 tex          : TEXCOORD1;
    uint   layer        : SV_RenderTargetArrayIndex;
};

[maxvertexcount(3)]
void main(triangle VSOutput input[3],
    inout TriangleStream<GSOutput> Stream
)
{
    for (uint i = 0; i < 3; i++)
    {
        GSOutput output;
        output.position = input[i].position;
        output.wsPosition = input[i].wsPosition;
        //output.tex = input[i].tex;
        output.layer = input[i].nInstanceID;
        Stream.Append(output);
    }
    Stream.RestartStrip();

    //GSOutput output;
    //output.position = input[0].position;
    //output.wsPosition = input[0].wsPosition;
    //output.tex = input[0].tex;
    //output.layer = input[0].nInstanceID;
    //Stream.Append(output);
}