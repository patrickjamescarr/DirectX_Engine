struct VSOutput {
    float4 position   : POSITION;
    float2 tex        : TEXCOORD;
    float3 position3D : TEXCOORD2;
    uint   nInstanceID  : SLICEINDEX;
};

struct GSOutput {
    float4 position     : SV_POSITION;
    float2 tex          : TEXCOORD;
    float3 position3D   : TEXCOORD2;
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
        output.tex = input[i].tex;
        output.position3D = input[i].position3D;
        output.layer = input[i].nInstanceID;
        Stream.Append(output);
    }
    Stream.RestartStrip();
}