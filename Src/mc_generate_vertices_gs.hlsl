// marching cubes generate verts geometry shader

#include "marching_cubes_header.hlsli"
#include "density.hlsli"

Texture3D density_vol : register(t0);
Texture2D shaderTexture : register(t1);
SamplerState LinearClamp : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer EdgeTableBuffer : register(b1)
{
    float edgeTable_c_buff[256];
    float4 isoLevel_c_buff;
};

cbuffer TriTableBuffer : register(b2)
{
    float triTable_c_buff[4096];
};

struct GSOutput
{
    float4 wsCoordAmbo : SV_POSITION;
    float3 wsNormal : NORMAL;
};

struct VSOutput
{
    float4 wsCellCoords[8]      : CELL;
    float3 densityVolCoords[8]  : DENSITY;
};

float4 ApplyMatrices(float4 coord)
{
    float4 outCoord;

    outCoord = mul(coord, worldMatrix);
    outCoord = mul(coord, viewMatrix);
    outCoord = mul(outCoord, projectionMatrix);

    return outCoord;
}

float4 ApplyViewProj(float4 coord)
{
    float4 outCoord;

    outCoord = mul(coord, viewMatrix);
    outCoord = mul(outCoord, projectionMatrix);

    return outCoord;
}
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
    //float d = 1.0 / 65.0f;
    float d = 1.0 / 10.0f;
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
    //GSOutput element;

    //element.wsCoordAmbo = ApplyMatrices(input[0].wsCellCoords[0]);
    //element.wsNormal = float3(1, 0, 1);
    //Stream.Append(element);

    //element.wsCoordAmbo = ApplyMatrices(input[0].wsCellCoords[1]);
    //element.wsNormal = float3(1, 0, 1);
    //Stream.Append(element);

    //element.wsCoordAmbo = ApplyMatrices(input[0].wsCellCoords[2]);
    //element.wsNormal = float3(1, 0, 1);
    //Stream.Append(element);

    //Stream.RestartStrip();

    gridCell cell;

    float dims = 65.0f;


    // bottom layer
    cell.p[0] = (float3)input[0].wsCellCoords[0];        // top left
    cell.p[1] = (float3)input[0].wsCellCoords[1];  // top right
    cell.p[2] = (float3)input[0].wsCellCoords[2];        // bottom right
    cell.p[3] = (float3)input[0].wsCellCoords[3];              // bottom left

    // top layer
    cell.p[4] = (float3)input[0].wsCellCoords[4];        // top left
    cell.p[5] = (float3)input[0].wsCellCoords[5];  // top right
    cell.p[6] = (float3)input[0].wsCellCoords[6];        // bottom right
    cell.p[7] = (float3)input[0].wsCellCoords[7];              // bottom left

    cell.densityValue[0] = density_vol.SampleLevel(LinearClamp, input[0].densityVolCoords[0], 0).x;
    cell.densityValue[1] = density_vol.SampleLevel(LinearClamp, input[0].densityVolCoords[1], 0).x;
    cell.densityValue[2] = density_vol.SampleLevel(LinearClamp, input[0].densityVolCoords[2], 0).x;
    cell.densityValue[3] = density_vol.SampleLevel(LinearClamp, input[0].densityVolCoords[3], 0).x;
    cell.densityValue[4] = density_vol.SampleLevel(LinearClamp, input[0].densityVolCoords[4], 0).x;
    cell.densityValue[5] = density_vol.SampleLevel(LinearClamp, input[0].densityVolCoords[5], 0).x;
    cell.densityValue[6] = density_vol.SampleLevel(LinearClamp, input[0].densityVolCoords[6], 0).x;
    cell.densityValue[7] = density_vol.SampleLevel(LinearClamp, input[0].densityVolCoords[7], 0).x;

    //GSOutput element;

    //element.wsCoordAmbo = ApplyViewProj(input[0].wsCellCoords[0]);
    //element.wsNormal = float3(1, 0, 1);
    //Stream.Append(element);

    //element.wsCoordAmbo = ApplyViewProj(input[0].wsCellCoords[1]);
    //element.wsNormal = float3(1, 0, 1);
    //Stream.Append(element);

    //element.wsCoordAmbo = ApplyViewProj(input[0].wsCellCoords[2]);
    //element.wsNormal = float3(1, 0, 1);
    //Stream.Append(element);

    //Stream.RestartStrip();

    /*
     Determine the index into the edge table which
     tells us which vertices are inside of the surface
    */

    float isoLevel = isoLevel_c_buff.x;

    int cubeindex = 0;
    if (cell.densityValue[0] < isoLevel) cubeindex |= 1;
    if (cell.densityValue[1] < isoLevel) cubeindex |= 2;
    if (cell.densityValue[2] < isoLevel) cubeindex |= 4;
    if (cell.densityValue[3] < isoLevel) cubeindex |= 8;
    if (cell.densityValue[4] < isoLevel) cubeindex |= 16;
    if (cell.densityValue[5] < isoLevel) cubeindex |= 32;
    if (cell.densityValue[6] < isoLevel) cubeindex |= 64;
    if (cell.densityValue[7] < isoLevel) cubeindex |= 128;

    uint edgeVal = (uint)edgeTable_c_buff[cubeindex].x;

    /* Cube is entirely in/out of the surface */
    if (edgeVal == 0)
        return;


    float3 vertlist[12];

    /* Find the vertices where the surface intersects the cube */
    if (edgeVal & 1)
    {
        vertlist[0] = VertexInterp(isoLevel, cell.p[0], cell.p[1], cell.densityValue[0], cell.densityValue[1]);
    }

    if (edgeVal & 2)
    {
        vertlist[1] = VertexInterp(isoLevel, cell.p[1], cell.p[2], cell.densityValue[1], cell.densityValue[2]);
    }

    if (edgeVal & 4)
    {
        vertlist[2] = VertexInterp(isoLevel, cell.p[2], cell.p[3], cell.densityValue[2], cell.densityValue[3]);
    }

    if (edgeVal & 8)
    {
        vertlist[3] = VertexInterp(isoLevel, cell.p[3], cell.p[0], cell.densityValue[3], cell.densityValue[0]);
    }

    if (edgeVal & 16)
    {
        vertlist[4] = VertexInterp(isoLevel, cell.p[4], cell.p[5], cell.densityValue[4], cell.densityValue[5]);
    }

    if (edgeVal & 32)
    {
        vertlist[5] = VertexInterp(isoLevel, cell.p[5], cell.p[6], cell.densityValue[5], cell.densityValue[6]);
    }

    if (edgeVal & 64)
    {
        vertlist[6] = VertexInterp(isoLevel, cell.p[6], cell.p[7], cell.densityValue[6], cell.densityValue[7]);
    }

    if (edgeVal & 128)
    {
        vertlist[7] = VertexInterp(isoLevel, cell.p[7], cell.p[4], cell.densityValue[7], cell.densityValue[4]);
    }

    if (edgeVal & 256)
    {
        vertlist[8] = VertexInterp(isoLevel, cell.p[0], cell.p[4], cell.densityValue[0], cell.densityValue[4]);
    }

    if (edgeVal & 512)
    {
        vertlist[9] = VertexInterp(isoLevel, cell.p[1], cell.p[5], cell.densityValue[1], cell.densityValue[5]);
    }

    if (edgeVal & 1024)
    {
        vertlist[10] = VertexInterp(isoLevel, cell.p[2], cell.p[6], cell.densityValue[2], cell.densityValue[6]);;
    }

    if (edgeVal & 2048)
    {
        vertlist[11] = VertexInterp(isoLevel, cell.p[3], cell.p[7], cell.densityValue[3], cell.densityValue[7]);
    }

    for (int i = 0; (int)triTable_c_buff[(cubeindex * 16) + i] != -1; i += 3)
    {
        float4 p0 = float4(vertlist[(int)triTable_c_buff[(cubeindex * 16) + i]].xyz, 1.0f);
        float4 p1 = float4(vertlist[(int)triTable_c_buff[(cubeindex * 16) + (i + 1)]].xyz, 1.0f);
        float4 p2 = float4(vertlist[(int)triTable_c_buff[(cubeindex * 16) + (i + 2)]].xyz, 1.0f);

        GSOutput element;

        element.wsCoordAmbo = ApplyViewProj(p0);
        element.wsNormal = CalculateNormal(p0.xyz);
        Stream.Append(element);

        element.wsCoordAmbo = ApplyViewProj(p1);
        element.wsNormal = CalculateNormal(p1.xyz);
        Stream.Append(element);

        element.wsCoordAmbo = ApplyViewProj(p2);
        element.wsNormal = CalculateNormal(p2.xyz);
        Stream.Append(element);

        Stream.RestartStrip();
    }
}

