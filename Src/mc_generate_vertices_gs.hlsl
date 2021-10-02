// marching cubes generate verts geometry shader
#include "marching_cubes_header.hlsli"

Texture3D density_vol : register(t0);
Texture2D shaderTexture : register(t1);
SamplerState LinearClamp : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CubeBuffer : register(b1)
{
    float  edgeTable_c_buff[256];
    float4 isoLevel_c_buff;
    float4 dim_c_buff;              // the dimention of the cube 
};

cbuffer TriTableBuffer : register(b2)
{
    float triTable_c_buff[4096];
};

// Fog buffer
cbuffer FogBuffer: register(b3)
{
    float4 fogEnd;
    float4 cameraPosition;
};

struct GSOutput
{
    float4 wsCoord  : SV_POSITION;
    float3 wsNormal     : NORMAL;
    float3 position3D   : TEXCOORD2;
    float  fogFactor    : FOG;
};

struct VSOutput
{
    float4 wsCellCoords[8]      : CELL;
    float3 densityVolCoords[8]  : DENSITY;
};

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

// Calculates the normal for a given point by taking the gradient of the density function
float3 CalculateNormal(float3 uvw)
{
    float d = 1.0 / dim_c_buff.x;

    float3 grad;
    grad.x = density_vol.SampleLevel(LinearClamp, uvw + float3(d, 0, 0), 0) - density_vol.SampleLevel(LinearClamp, uvw + float3(-d, 0, 0), 0);
    grad.y = density_vol.SampleLevel(LinearClamp, uvw + float3(0, d, 0), 0) - density_vol.SampleLevel(LinearClamp, uvw + float3(0, -d, 0), 0);
    grad.z = density_vol.SampleLevel(LinearClamp, uvw + float3(0, 0, d), 0) - density_vol.SampleLevel(LinearClamp, uvw + float3(0, 0, -d), 0);

    return -normalize(grad);
}


// calculate linear fog
float CalculateLinearFogFactor(float3 vertexPosition, float3 camPosition)
{
    float fogFactor;

    float camDistanceToVertex = length(vertexPosition - camPosition);

    if (camDistanceToVertex >= fogEnd.x)
    {
        return 0;
    }

    fogFactor = saturate((fogEnd.x - camDistanceToVertex) / fogEnd.x);

    return fogFactor;
}

[maxvertexcount(15)]
void main(
    point VSOutput input[1],
    inout TriangleStream< GSOutput > Stream
)
{
    // a grid cell with 8 vertices.  one for each corner of the cube
    gridCell cell;

    for (int i = 0; i < 8; i++)
    {
        // the vertex world space position
        cell.p[i] = (float3)input[0].wsCellCoords[i];

        // sample the density value for this vertex from the density volume texture
        cell.densityValue[i] = density_vol.SampleLevel(LinearClamp, input[0].densityVolCoords[i], 0).x;

        // calculate the vertex normal using the gradient of the density function
        cell.normal[i] = CalculateNormal(input[0].densityVolCoords[i]);
    }

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

    float3 vertlist[12] = { float3(0,0,0), float3(0,0,0), float3(0,0,0), float3(0,0,0), float3(0,0,0), float3(0,0,0), float3(0,0,0), float3(0,0,0), float3(0,0,0), float3(0,0,0), float3(0,0,0), float3(0,0,0) };
    float3 normList[12] = { float3(0,0,0), float3(0,0,0), float3(0,0,0), float3(0,0,0), float3(0,0,0), float3(0,0,0), float3(0,0,0), float3(0,0,0), float3(0,0,0), float3(0,0,0), float3(0,0,0), float3(0,0,0) };

    /* Find the vertices and their normals where the surface intersects the cube */
    if (edgeVal & 1)
    {
        vertlist[0] = VertexInterp(isoLevel, cell.p[0], cell.p[1], cell.densityValue[0], cell.densityValue[1]);
        normList[0] = VertexInterp(isoLevel, cell.normal[0], cell.normal[1], cell.densityValue[0], cell.densityValue[1]);
    }

    if (edgeVal & 2)
    {
        vertlist[1] = VertexInterp(isoLevel, cell.p[1], cell.p[2], cell.densityValue[1], cell.densityValue[2]);
        normList[1] = VertexInterp(isoLevel, cell.normal[1], cell.normal[2], cell.densityValue[1], cell.densityValue[2]);
    }

    if (edgeVal & 4)
    {
        vertlist[2] = VertexInterp(isoLevel, cell.p[2], cell.p[3], cell.densityValue[2], cell.densityValue[3]);
        normList[2] = VertexInterp(isoLevel, cell.normal[2], cell.normal[3], cell.densityValue[2], cell.densityValue[3]);
    }

    if (edgeVal & 8)
    {
        vertlist[3] = VertexInterp(isoLevel, cell.p[3], cell.p[0], cell.densityValue[3], cell.densityValue[0]);
        normList[3] = VertexInterp(isoLevel, cell.normal[3], cell.normal[0], cell.densityValue[3], cell.densityValue[0]);
    }

    if (edgeVal & 16)
    {
        vertlist[4] = VertexInterp(isoLevel, cell.p[4], cell.p[5], cell.densityValue[4], cell.densityValue[5]);
        normList[4] = VertexInterp(isoLevel, cell.normal[4], cell.normal[5], cell.densityValue[4], cell.densityValue[5]);
    }

    if (edgeVal & 32)
    {
        vertlist[5] = VertexInterp(isoLevel, cell.p[5], cell.p[6], cell.densityValue[5], cell.densityValue[6]);
        normList[5] = VertexInterp(isoLevel, cell.normal[5], cell.normal[6], cell.densityValue[5], cell.densityValue[6]);
    }

    if (edgeVal & 64)
    {
        vertlist[6] = VertexInterp(isoLevel, cell.p[6], cell.p[7], cell.densityValue[6], cell.densityValue[7]);
        normList[6] = VertexInterp(isoLevel, cell.normal[6], cell.normal[7], cell.densityValue[6], cell.densityValue[7]);
    }

    if (edgeVal & 128)
    {
        vertlist[7] = VertexInterp(isoLevel, cell.p[7], cell.p[4], cell.densityValue[7], cell.densityValue[4]);
        normList[7] = VertexInterp(isoLevel, cell.normal[7], cell.normal[4], cell.densityValue[7], cell.densityValue[4]);
    }

    if (edgeVal & 256)
    {
        vertlist[8] = VertexInterp(isoLevel, cell.p[0], cell.p[4], cell.densityValue[0], cell.densityValue[4]);
        normList[8] = VertexInterp(isoLevel, cell.normal[0], cell.normal[4], cell.densityValue[0], cell.densityValue[4]);
    }

    if (edgeVal & 512)
    {
        vertlist[9] = VertexInterp(isoLevel, cell.p[1], cell.p[5], cell.densityValue[1], cell.densityValue[5]);
        normList[9] = VertexInterp(isoLevel, cell.normal[1], cell.normal[5], cell.densityValue[1], cell.densityValue[5]);
    }

    if (edgeVal & 1024)
    {
        vertlist[10] = VertexInterp(isoLevel, cell.p[2], cell.p[6], cell.densityValue[2], cell.densityValue[6]);
        normList[10] = VertexInterp(isoLevel, cell.normal[2], cell.normal[6], cell.densityValue[2], cell.densityValue[6]);
    }

    if (edgeVal & 2048)
    {
        vertlist[11] = VertexInterp(isoLevel, cell.p[3], cell.p[7], cell.densityValue[3], cell.densityValue[7]);
        normList[11] = VertexInterp(isoLevel, cell.normal[3], cell.normal[7], cell.densityValue[3], cell.densityValue[7]);
    }

    float4 camWS;

    camWS = mul(cameraPosition, worldMatrix);


    for (int i = 0; (int)triTable_c_buff[(cubeindex * 16) + i] != -1; i += 3)
    {
        int t0_index = (cubeindex * 16) + i;
        int t1_index = (cubeindex * 16) + (i + 1);
        int t2_index = (cubeindex * 16) + (i + 2);

        int v0_index = (int)triTable_c_buff[t0_index];
        int v1_index = (int)triTable_c_buff[t1_index];
        int v2_index = (int)triTable_c_buff[t2_index];

        float4 p0 = float4(vertlist[v0_index].xyz, 1.0f);
        float4 p1 = float4(vertlist[v1_index].xyz, 1.0f);
        float4 p2 = float4(vertlist[v2_index].xyz, 1.0f);

        GSOutput element;

        element.wsCoord = ApplyViewProj(p0);
        element.position3D = p0;
        element.wsNormal = normList[v0_index];
        element.fogFactor = CalculateLinearFogFactor(p0, camWS.xyz); // Calculate linear fog.
        Stream.Append(element);

        element.wsCoord = ApplyViewProj(p1);
        element.position3D = p1;
        element.wsNormal = normList[v1_index];
        element.fogFactor = CalculateLinearFogFactor(p1, camWS.xyz); // Calculate linear fog.
        Stream.Append(element);

        element.wsCoord = ApplyViewProj(p2);
        element.position3D = p2;
        element.wsNormal = normList[v2_index];
        element.fogFactor = CalculateLinearFogFactor(p2, camWS.xyz); // Calculate linear fog.
        Stream.Append(element);

        Stream.RestartStrip();
    }
    
}

