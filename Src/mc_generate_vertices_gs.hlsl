// marching cubes generate verts geometry shader

#include "marching_cubes_header.hlsli"
#include "density.hlsli"

Texture3D density_vol : register(t0);
SamplerState LinearClamp : register(s0);

struct GSOutput
{
    float4 wsCoordAmbo : SV_POSITION;
    float3 wsNormal : NORMAL;
};

struct VSOutput
{
    float4 wsPosition : POSITION;
};

/*
    Linearly interpolate the position where an isosurface cuts
    an edge between two vertices, each with their own scalar value
*/
float3 VertexInterp(float isolevel, float3 p1, float3 p2, float valp1, float valp2)
{
    float mu;
    float3 p;

    if (abs(isolevel - valp1) < 0.00001)
        return(p1);
    if (abs(isolevel - valp2) < 0.00001)
        return(p2);
    if (abs(valp1 - valp2) < 0.00001)
        return(p1);

    mu = (isolevel - valp1) / (valp2 - valp1);
    p.x = p1.x + mu * (p2.x - p1.x);
    p.y = p1.y + mu * (p2.y - p1.y);
    p.z = p1.z + mu * (p2.z - p1.z);

    return(p);
}

float3 CalculateNormal(float3 uvw)
{
    float d = 1.0 / 65.0f;
    float3 grad;
    grad.x = density_vol.SampleLevel(LinearClamp, uvw + float3(d, 0, 0), 0) - density_vol.SampleLevel(LinearClamp, uvw + float3(-d, 0, 0), 0);
    grad.y = density_vol.SampleLevel(LinearClamp, uvw + float3(0, d, 0), 0) - density_vol.SampleLevel(LinearClamp, uvw + float3(0, -d, 0), 0);
    grad.z = density_vol.SampleLevel(LinearClamp, uvw + float3(0, 0, d), 0) - density_vol.SampleLevel(LinearClamp, uvw + float3(0, 0, -d), 0);

    return -normalize(grad);
}

[maxvertexcount(15)]
void main(
    point VSOutput input[1],
    inout TriangleStream< GSOutput > Stream
)
{
    float4 ws = input[0].wsPosition;

    //gridCell cell;

    //float dims = 1.0f;

    //float isolevel = 5;

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


    GSOutput element;

    element.wsCoordAmbo = float4(ws.x, ws.y, ws.z, ws.w);
    element.wsNormal = float3(0, 0, 1);
    Stream.Append(element);

    element.wsCoordAmbo = float4(ws.x + 3, ws.y + 6, ws.z, ws.w);
    element.wsNormal = float3(0, 0, 1);
    Stream.Append(element);

    element.wsCoordAmbo = float4(ws.x + 6, ws.y, ws.z, ws.w);
    element.wsNormal = float3(0, 0, 1);
    Stream.Append(element);



    //GSOutput element;

    //element.wsCoordAmbo = float4(ws.x, ws.y, ws.z, 0);
    //element.wsNormal = float3(1, 0, 1);
    //Stream.Append(element);

    //element.wsCoordAmbo = float4(ws.x + 1, ws.y + 1, ws.z, 0);
    //element.wsNormal = float3(1, 0, 1);
    //Stream.Append(element);

    //element.wsCoordAmbo = float4(ws.x + 1, ws.y, ws.z, 0);
    //element.wsNormal = float3(1, 0, 1);
    //Stream.Append(element);

    //Stream.RestartStrip();

    //gridCell cell;

    //float dims = 65.0f;

    //float isolevel = 5;

    //// bottom layer
    //cell.p[0] = float3(ws.x, (ws.y + 1), ws.z) / dims;        // top left
    //cell.p[1] = float3((ws.x + 1), (ws.y + 1), ws.z) / dims;  // top right
    //cell.p[2] = float3((ws.x + 1), ws.y, ws.z) / dims;        // bottom right
    //cell.p[3] = float3(ws.x, ws.y, ws.z) / dims;              // bottom left

    //// top layer
    //cell.p[4] = float3(ws.x, (ws.y + 1), (ws.z + 1)) / dims;        // top left
    //cell.p[5] = float3((ws.x + 1), (ws.y + 1), (ws.z + 1)) / dims;  // top right
    //cell.p[6] = float3((ws.x + 1), ws.y, (ws.z + 1)) / dims;        // bottom right
    //cell.p[7] = float3(ws.x, ws.y, (ws.z + 1)) / dims;              // bottom left

    //cell.densityValue[0] = density_vol.SampleLevel(LinearClamp, cell.p[0], 0).x;
    //cell.densityValue[1] = density_vol.SampleLevel(LinearClamp, cell.p[1], 0).x;
    //cell.densityValue[2] = density_vol.SampleLevel(LinearClamp, cell.p[2], 0).x;
    //cell.densityValue[3] = density_vol.SampleLevel(LinearClamp, cell.p[3], 0).x;
    //cell.densityValue[4] = density_vol.SampleLevel(LinearClamp, cell.p[4], 0).x;
    //cell.densityValue[5] = density_vol.SampleLevel(LinearClamp, cell.p[5], 0).x;
    //cell.densityValue[6] = density_vol.SampleLevel(LinearClamp, cell.p[6], 0).x;
    //cell.densityValue[7] = density_vol.SampleLevel(LinearClamp, cell.p[7], 0).x;

    ///*
    // Determine the index into the edge table which
    // tells us which vertices are inside of the surface
    //*/

    //int cubeindex = 0;
    //if (cell.densityValue[0] < isolevel) cubeindex |= 1;
    //if (cell.densityValue[1] < isolevel) cubeindex |= 2;
    //if (cell.densityValue[2] < isolevel) cubeindex |= 4;
    //if (cell.densityValue[3] < isolevel) cubeindex |= 8;
    //if (cell.densityValue[4] < isolevel) cubeindex |= 16;
    //if (cell.densityValue[5] < isolevel) cubeindex |= 32;
    //if (cell.densityValue[6] < isolevel) cubeindex |= 64;
    //if (cell.densityValue[7] < isolevel) cubeindex |= 128;

    ///* Cube is entirely in/out of the surface */
    //if (edgeTable[cubeindex] == 0)
    //    return;

    //float3 vertlist[12];

    ///* Find the vertices where the surface intersects the cube */
    //if (edgeTable[cubeindex] & 1)
    //{
    //    vertlist[0] = VertexInterp(isolevel, cell.p[0], cell.p[1], cell.densityValue[0], cell.densityValue[1]);
    //}

    //if (edgeTable[cubeindex] & 2)
    //{
    //    vertlist[1] = VertexInterp(isolevel, cell.p[1], cell.p[2], cell.densityValue[1], cell.densityValue[2]);
    //}

    //if (edgeTable[cubeindex] & 4)
    //{
    //    vertlist[2] = VertexInterp(isolevel, cell.p[2], cell.p[3], cell.densityValue[2], cell.densityValue[3]);
    //}

    //if (edgeTable[cubeindex] & 8)
    //{
    //    vertlist[3] = VertexInterp(isolevel, cell.p[3], cell.p[0], cell.densityValue[3], cell.densityValue[0]);
    //}

    //if (edgeTable[cubeindex] & 16)
    //{
    //    vertlist[4] = VertexInterp(isolevel, cell.p[4], cell.p[5], cell.densityValue[4], cell.densityValue[5]);
    //}

    //if (edgeTable[cubeindex] & 32)
    //{
    //    vertlist[5] = VertexInterp(isolevel, cell.p[5], cell.p[6], cell.densityValue[5], cell.densityValue[6]);
    //}

    //if (edgeTable[cubeindex] & 64)
    //{
    //    vertlist[6] = VertexInterp(isolevel, cell.p[6], cell.p[7], cell.densityValue[6], cell.densityValue[7]);
    //}

    //if (edgeTable[cubeindex] & 128)
    //{
    //    vertlist[7] = VertexInterp(isolevel, cell.p[7], cell.p[4], cell.densityValue[7], cell.densityValue[4]);
    //}

    //if (edgeTable[cubeindex] & 256)
    //{
    //    vertlist[8] = VertexInterp(isolevel, cell.p[0], cell.p[4], cell.densityValue[0], cell.densityValue[4]);
    //}

    //if (edgeTable[cubeindex] & 512)
    //{
    //    vertlist[9] = VertexInterp(isolevel, cell.p[1], cell.p[5], cell.densityValue[1], cell.densityValue[5]);
    //}

    //if (edgeTable[cubeindex] & 1024)
    //{
    //    vertlist[10] = VertexInterp(isolevel, cell.p[2], cell.p[6], cell.densityValue[2], cell.densityValue[6]);;
    //}

    //if (edgeTable[cubeindex] & 2048)
    //{
    //    vertlist[11] = VertexInterp(isolevel, cell.p[3], cell.p[7], cell.densityValue[3], cell.densityValue[7]);
    //}

    //for (int i = 0; triTable[cubeindex][i] != -1; i += 3)
    //{
    //    float4 p0 = float4(vertlist[triTable[cubeindex][i]].xyz, 0);
    //    float4 p1 = float4(vertlist[triTable[cubeindex][i + 1]].xyz, 0);
    //    float4 p2 = float4(vertlist[triTable[cubeindex][i + 2]].xyz, 0);

    //    GSOutput element;

    //    element.wsCoordAmbo = p0;
    //    element.wsNormal = CalculateNormal(p0.xyz);
    //    Stream.Append(element);

    //    element.wsCoordAmbo = p1;
    //    element.wsNormal = CalculateNormal(p1.xyz);
    //    Stream.Append(element);

    //    element.wsCoordAmbo = p2;
    //    element.wsNormal = CalculateNormal(p2.xyz);
    //    Stream.Append(element);

    //    Stream.RestartStrip();
    //}
}

