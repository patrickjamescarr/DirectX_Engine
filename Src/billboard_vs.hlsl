cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CameraBuffer : register(b1)
{
    float3 cameraPosition;
};

struct VertexIn
{
    float3 PosW : POSITION;
};

struct VertexOut
{
    float4 CenterW : POSITION;
    float3 CamPosW: TEXCOORD;
    float2 SizeW : SIZE;
    matrix viewMatrix : MATRIX0;
    matrix projMatrix : PMATRIX1;
};

VertexOut main(VertexIn vin, uint vertID : SV_VertexID)
{
    VertexOut vout;

    // Just pass data over to geometry shader.
    vout.SizeW = 0.5f;

    vout.CenterW = mul(vin.PosW, worldMatrix);

    vout.CamPosW = mul(cameraPosition, worldMatrix);

    vout.viewMatrix = viewMatrix;
    vout.projMatrix = projectionMatrix;

    return vout;
}