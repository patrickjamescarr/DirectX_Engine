#include "pch.h"
#include "MarchingCubes.h"
#include "SimplexNoise.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

MeshObject MarchingCubes::GenerateTerrain(int xSize, int ySize, int zSize)
{
    MeshObject mesh;
    mesh.vertexCount = 0;

    SimplexNoise noise;
    float scale = 0.03;
    float multiplier = 10.f;

    std::vector<double> vals;

    int indexCount = 0;

    for (int k = 0; k < (ySize - 1); k++)
    {
        for (int j = 0; j < (zSize - 1); j++)
        {
            for (int i = 0; i < (xSize - 1); i++)
            {
                GridCell gridCell;

                auto v1 = Vector3(i * scale, j * scale, k * scale);                     // bottom left
                auto v2 = Vector3((i + 1) * scale, j * scale, k * scale);               // top left
                auto v3 = Vector3(i * scale, (j + 1) * scale, k * scale);               // top right
                auto v4 = Vector3((i + 1) * scale, (j + 1) * scale, k * scale);         // bottom right
                auto v5 = Vector3(i * scale, j * scale, (k + 1) * scale);               // bottom left
                auto v6 = Vector3((i + 1) * scale, j * scale, (k + 1) * scale);         // top left
                auto v7 = Vector3(i * scale, (j + 1) * scale, (k + 1) * scale);         // top right
                auto v8 = Vector3((i + 1) * scale, (j + 1) * scale, (k + 1) * scale);   // bottom right

                gridCell.p[0] = v1;
                gridCell.p[1] = v2;
                gridCell.p[2] = v3;
                gridCell.p[3] = v4;
                gridCell.p[4] = v5;
                gridCell.p[5] = v6;
                gridCell.p[6] = v7;
                gridCell.p[7] = v8;

                //lower layer
                gridCell.val[0] = noise.Generate(v1.x, v1.y, v1.z) * multiplier; 
                gridCell.val[1] = noise.Generate(v2.x, v2.y, v2.z) * multiplier; 
                gridCell.val[2] = noise.Generate(v3.x, v3.y, v3.z) * multiplier; 
                gridCell.val[3] = noise.Generate(v4.x, v4.y, v4.z) * multiplier; 

                //upper layer
                gridCell.val[4] = noise.Generate(v5.x, v5.y, v5.z) * multiplier; 
                gridCell.val[5] = noise.Generate(v6.x, v6.y, v6.z) * multiplier; 
                gridCell.val[6] = noise.Generate(v7.x, v7.y, v7.z) * multiplier; 
                gridCell.val[7] = noise.Generate(v8.x, v8.y, v8.z) * multiplier; 

                std::vector<Triangle> triangles;

                auto triangleCount = Polygonise(gridCell, 6, triangles);

                mesh.vertexCount += triangleCount * 3;

                for (auto& tri : triangles)
                {
                    auto normal = (tri.p[0] - tri.p[2]).Cross(tri.p[2] - tri.p[1]);

                    AddMeshVertex(tri.p[0], normal, mesh, indexCount);
                    AddMeshVertex(tri.p[1], normal, mesh, indexCount);
                    AddMeshVertex(tri.p[2], normal, mesh, indexCount);
                }

            }
        }
    }

    mesh.indexCount = mesh.vertexCount;

    return mesh;
}

void MarchingCubes::AddMeshVertex(Vector3 Vertex, Vector3 normal, MeshObject &mesh, int &indexCount)
{
    VertexPositionNormalTexture vertex;
    vertex.position = Vertex;
    vertex.normal = normal;
    vertex.textureCoordinate = Vector2::Zero;
    mesh.verticies.push_back(vertex);
    mesh.indices.push_back(indexCount);
    indexCount++;
}

/*
   Given a grid cell and an isolevel, calculate the triangular
   facets required to represent the isosurface through the cell.
   Return the number of triangular facets, the array "triangles"
   will be loaded up with the vertices at most 5 triangular facets.
    0 will be returned if the grid cell is either totally above
   or totally below the isolevel.
*/
int MarchingCubes::Polygonise(GridCell grid, double isolevel, std::vector<Triangle>& triangles)
{
    /*
     Determine the index into the edge table which
     tells us which vertices are inside of the surface
    */
    int cubeindex = 0;
    if (grid.val[0] < isolevel) cubeindex |= 1;
    if (grid.val[1] < isolevel) cubeindex |= 2;
    if (grid.val[2] < isolevel) cubeindex |= 4;
    if (grid.val[3] < isolevel) cubeindex |= 8;
    if (grid.val[4] < isolevel) cubeindex |= 16;
    if (grid.val[5] < isolevel) cubeindex |= 32;
    if (grid.val[6] < isolevel) cubeindex |= 64;
    if (grid.val[7] < isolevel) cubeindex |= 128;

    /* Cube is entirely in/out of the surface */
    if (edgeTable[cubeindex] == 0)
        return(0);

    Vector3 vertlist[12];

    /* Find the vertices where the surface intersects the cube */
    if (edgeTable[cubeindex] & 1)
        vertlist[0] =
        VertexInterp(isolevel, grid.p[0], grid.p[1], grid.val[0], grid.val[1]);
    if (edgeTable[cubeindex] & 2)
        vertlist[1] =
        VertexInterp(isolevel, grid.p[1], grid.p[2], grid.val[1], grid.val[2]);
    if (edgeTable[cubeindex] & 4)
        vertlist[2] =
        VertexInterp(isolevel, grid.p[2], grid.p[3], grid.val[2], grid.val[3]);
    if (edgeTable[cubeindex] & 8)
        vertlist[3] =
        VertexInterp(isolevel, grid.p[3], grid.p[0], grid.val[3], grid.val[0]);
    if (edgeTable[cubeindex] & 16)
        vertlist[4] =
        VertexInterp(isolevel, grid.p[4], grid.p[5], grid.val[4], grid.val[5]);
    if (edgeTable[cubeindex] & 32)
        vertlist[5] =
        VertexInterp(isolevel, grid.p[5], grid.p[6], grid.val[5], grid.val[6]);
    if (edgeTable[cubeindex] & 64)
        vertlist[6] =
        VertexInterp(isolevel, grid.p[6], grid.p[7], grid.val[6], grid.val[7]);
    if (edgeTable[cubeindex] & 128)
        vertlist[7] =
        VertexInterp(isolevel, grid.p[7], grid.p[4], grid.val[7], grid.val[4]);
    if (edgeTable[cubeindex] & 256)
        vertlist[8] =
        VertexInterp(isolevel, grid.p[0], grid.p[4], grid.val[0], grid.val[4]);
    if (edgeTable[cubeindex] & 512)
        vertlist[9] =
        VertexInterp(isolevel, grid.p[1], grid.p[5], grid.val[1], grid.val[5]);
    if (edgeTable[cubeindex] & 1024)
        vertlist[10] =
        VertexInterp(isolevel, grid.p[2], grid.p[6], grid.val[2], grid.val[6]);
    if (edgeTable[cubeindex] & 2048)
        vertlist[11] =
        VertexInterp(isolevel, grid.p[3], grid.p[7], grid.val[3], grid.val[7]);

    /* Create the triangle */
    int ntriang = 0;
    for (int i = 0; triTable[cubeindex][i] != -1; i += 3) {
        Triangle t;
        t.p[0] = vertlist[triTable[cubeindex][i]];
        t.p[1] = vertlist[triTable[cubeindex][i + 1]];
        t.p[2] = vertlist[triTable[cubeindex][i + 2]];
        triangles.push_back(t);
        ntriang++;
    }

    return(ntriang);
}

/*
    Linearly interpolate the position where an isosurface cuts
    an edge between two vertices, each with their own scalar value
*/
DirectX::SimpleMath::Vector3 MarchingCubes::VertexInterp(double isolevel, Vector3 p1, Vector3 p2, double valp1, double valp2)
{
    double mu;
    Vector3 p;

    if (std::abs(isolevel - valp1) < 0.00001)
        return(p1);
    if (std::abs(isolevel - valp2) < 0.00001)
        return(p2);
    if (std::abs(valp1 - valp2) < 0.00001)
        return(p1);

    mu = (isolevel - valp1) / (valp2 - valp1);
    p.x = p1.x + mu * (p2.x - p1.x);
    p.y = p1.y + mu * (p2.y - p1.y);
    p.z = p1.z + mu * (p2.z - p1.z);

    return(p);
}
