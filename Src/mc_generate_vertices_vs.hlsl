// marching cubes vertex shader

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VSInput
{
    float4 position : POSITION;
};

struct VSOutput
{
    float4 wsPosition : POSITION;
};

VSOutput main(VSInput input)
{
    VSOutput output;

    input.position.w = 1.0f;

    // world position of vertex
    //output.wsPosition = mul(input.position, worldMatrix);

    output.wsPosition = mul(input.position, worldMatrix);
    output.wsPosition = mul(output.wsPosition, viewMatrix);
    output.wsPosition = mul(output.wsPosition, projectionMatrix);


    // calculate these here then pass to geometry shader
    //// front face
//cell.p[0] = float3(ws.x, (ws.y + 1), ws.z) / dims;        // top left
//cell.p[1] = float3((ws.x + 1), (ws.y + 1), ws.z) / dims;  // top right
//cell.p[2] = float3((ws.x + 1), ws.y, ws.z) / dims;        // bottom right
//cell.p[3] = float3(ws.x, ws.y, ws.z) / dims;              // bottom left

//// back face
//cell.p[4] = float3(ws.x, (ws.y + 1), (ws.z + 1)) / dims;        // top left
//cell.p[5] = float3((ws.x + 1), (ws.y + 1), (ws.z + 1)) / dims;  // top right
//cell.p[6] = float3((ws.x + 1), ws.y, (ws.z + 1)) / dims;        // bottom right
//cell.p[7] = float3(ws.x, ws.y, (ws.z + 1)) / dims;              // bottom left



    return output;
}