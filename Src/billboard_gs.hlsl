cbuffer cbFixed
{
    //
    // Compute texture coordinates to stretch texture over quad.
    //
    float2 gTexC[4] =
    {
    float2(0.0f, 1.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 1.0f),
    float2(1.0f, 0.0f)
    };
};

struct VertexOut
{
    float4 CenterW : POSITION;
    float3 CamPosW: TEXCOORD;
    float2 SizeW : SIZE;
    matrix viewMatrix : MATRIX0;
    matrix projMatrix : PMATRIX1;
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

// We expand each point into a quad (4 vertices), so the maximum number
// of vertices we output per geometry shader invocation is 4.
[maxvertexcount(4)]
void main(point VertexOut gin[1],
    uint primID : SV_PrimitiveID,
    inout TriangleStream<GeoOut> triStream)
{
    //
    // Compute the local coordinate system of the sprite relative to the
    // world space such that the billboard is aligned with the y-axis
    // and faces the eye.
    //
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 look = gin[0].CamPosW - gin[0].CenterW;
    look.y = 0.0f; // y-axis aligned, so project to xz-plane
    look = normalize(look);
    float3 right = cross(up, look);

    //
    // Compute triangle strip vertices (quad) in world space.
    //
    float halfWidth = 0.5f*gin[0].SizeW.x;
    float halfHeight = 0.5f*gin[0].SizeW.y;

    float4 v[4];
    v[0] = float4(gin[0].CenterW + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(gin[0].CenterW + halfWidth * right + halfHeight * up, 1.0f);
    v[2] = float4(gin[0].CenterW - halfWidth * right - halfHeight * up, 1.0f);
    v[3] = float4(gin[0].CenterW - halfWidth * right + halfHeight * up, 1.0f);

    //
    // Transform quad vertices to world space and output
    // them as a triangle strip.
    //
    GeoOut gout;
    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        gout.PosH = mul(v[i], gin[0].viewMatrix);
        gout.PosH = mul(gout.PosH, gin[0].projMatrix);
        gout.PosW = v[i].xyz;
        gout.NormalW = look;
        gout.Tex = gTexC[i];
        gout.PrimID = primID;
        gout.CamPosW = gin[0].CamPosW;
        triStream.Append(gout);
    }
}