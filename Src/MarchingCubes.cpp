#include "pch.h"
#include "MarchingCubes.h"
#include <sstream> 
#include <iostream>
#include <map>

using namespace DirectX;
using namespace DirectX::SimpleMath;

MarchingCubes::MarchingCubes(DensityFunction * function) 
    : m_function(function)
{
}

MeshObject MarchingCubes::Generate(int xSize, int ySize, int zSize)
{
    MeshObject mesh;

    int indexCount = 0;

    for (int j = 0; j < (ySize - 1); j++)
    {
        for (int k = 0; k < (zSize - 1); k++)
        {
            for (int i = 0; i < (xSize - 1); i++)
            {
                GridCell gridCell;

                auto verts = GenerateCubeVerticies(i, j, k);

                for (int i = 0; i < verts.size(); i++)
                {
                    gridCell.point[i] = verts[i];
                    gridCell.densityValue[i] = m_function->execute(verts[i].x, verts[i].y, verts[i].z);

                    //std::wstringstream ss;
                    //ss << gridCell.densityValue[i] << "\n";
                    //
                    //OutputDebugString(ss.str().c_str());
                }


                std::vector<Triangle> triangles;

                mesh.vertexCount += Polygonise(gridCell, m_isoLevel, triangles) * 3;

                for (auto& tri : triangles)
                {
                    // auto normal = (tri.p[0] - tri.p[2]).Cross(tri.p[2] - tri.p[1]);

                    AddMeshVertex(tri.p[0], mesh, indexCount);
                    AddMeshVertex(tri.p[1], mesh, indexCount);
                    AddMeshVertex(tri.p[2], mesh, indexCount);
                }
            }
        }
    }

    mesh.indexCount = mesh.vertexCount;

    return mesh;
}

void MarchingCubes::DisplayUI()
{

}

void MarchingCubes::CalculateGradient(const DirectX::SimpleMath::Vector3 & pos, DirectX::SimpleMath::Vector3 & normal)
{
    float value = m_function->execute(pos.x, pos.y, pos.z);

    float unit = 1 * m_scale;

    normal.x = ((m_function->execute(pos.x + unit, pos.y, pos.z)) - m_function->execute(pos.x - unit, pos.y, pos.z));
    normal.y = ((m_function->execute(pos.x, pos.y + unit, pos.z)) - m_function->execute(pos.x, pos.y - unit, pos.z));
    normal.z = ((m_function->execute(pos.x, pos.y, pos.z + unit)) - m_function->execute(pos.x, pos.y, pos.z - unit));

    normal.Normalize();
}

void MarchingCubes::AddMeshVertex(
    Vector3 vertex, 
    MeshObject &mesh, 
    int &indexCount
)
{
    VertexPositionNormalTexture v;
    Vector3 normal;

    CalculateGradient(vertex, normal);

    v.position = vertex;
    v.normal = -normal;
    v.textureCoordinate = Vector2::Zero;

    mesh.verticies.push_back(v);
    mesh.indices.push_back(indexCount);
    indexCount++;
}

std::vector<DirectX::SimpleMath::Vector3> MarchingCubes::GenerateCubeVerticies(int i, int j, int k)
{
    std::vector<DirectX::SimpleMath::Vector3> verts;

    // bottom layer
    verts.push_back(Vector3(i, (j + 1), k) * m_scale);        // top left
    verts.push_back(Vector3((i + 1), (j + 1), k) * m_scale);  // top right
    verts.push_back(Vector3((i + 1), j, k) * m_scale);        // bottom right
    verts.push_back(Vector3(i, j, k) * m_scale);              // bottom left

    // top layer
    verts.push_back(Vector3(i, (j + 1), (k + 1)) * m_scale);        // top left
    verts.push_back(Vector3((i + 1), (j + 1), (k + 1)) * m_scale);  // top right
    verts.push_back(Vector3((i + 1), j, (k + 1)) * m_scale);        // bottom right
    verts.push_back(Vector3(i, j, (k + 1)) * m_scale);              // bottom left

    return verts;
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
    if (grid.densityValue[0] < isolevel) cubeindex |= 1;
    if (grid.densityValue[1] < isolevel) cubeindex |= 2;
    if (grid.densityValue[2] < isolevel) cubeindex |= 4;
    if (grid.densityValue[3] < isolevel) cubeindex |= 8;
    if (grid.densityValue[4] < isolevel) cubeindex |= 16;
    if (grid.densityValue[5] < isolevel) cubeindex |= 32;
    if (grid.densityValue[6] < isolevel) cubeindex |= 64;
    if (grid.densityValue[7] < isolevel) cubeindex |= 128;

    /* Cube is entirely in/out of the surface */
    if (edgeTable[cubeindex] == 0)
        return(0);

    Vector3 vertlist[12];
 
    /* Find the vertices where the surface intersects the cube */
    if (edgeTable[cubeindex] & 1)
    {
        vertlist[0] = VertexInterp(isolevel, grid.point[0], grid.point[1], grid.densityValue[0], grid.densityValue[1]);
    }

    if (edgeTable[cubeindex] & 2)
    {
        vertlist[1] = VertexInterp(isolevel, grid.point[1], grid.point[2], grid.densityValue[1], grid.densityValue[2]);
    }

    if (edgeTable[cubeindex] & 4)
    {
        vertlist[2] = VertexInterp(isolevel, grid.point[2], grid.point[3], grid.densityValue[2], grid.densityValue[3]);
    }

    if (edgeTable[cubeindex] & 8)
    {
        vertlist[3] = VertexInterp(isolevel, grid.point[3], grid.point[0], grid.densityValue[3], grid.densityValue[0]);
    }

    if (edgeTable[cubeindex] & 16)
    {
        vertlist[4] = VertexInterp(isolevel, grid.point[4], grid.point[5], grid.densityValue[4], grid.densityValue[5]);
    }

    if (edgeTable[cubeindex] & 32)
    {
        vertlist[5] = VertexInterp(isolevel, grid.point[5], grid.point[6], grid.densityValue[5], grid.densityValue[6]);
    }

    if (edgeTable[cubeindex] & 64)
    {
        vertlist[6] = VertexInterp(isolevel, grid.point[6], grid.point[7], grid.densityValue[6], grid.densityValue[7]);
    }

    if (edgeTable[cubeindex] & 128)
    {
        vertlist[7] = VertexInterp(isolevel, grid.point[7], grid.point[4], grid.densityValue[7], grid.densityValue[4]);
    }

    if (edgeTable[cubeindex] & 256)
    {
        vertlist[8] = VertexInterp(isolevel, grid.point[0], grid.point[4], grid.densityValue[0], grid.densityValue[4]);
    }

    if (edgeTable[cubeindex] & 512)
    {
        vertlist[9] = VertexInterp(isolevel, grid.point[1], grid.point[5], grid.densityValue[1], grid.densityValue[5]);
    }

    if (edgeTable[cubeindex] & 1024)
    {
        vertlist[10] = VertexInterp(isolevel, grid.point[2], grid.point[6], grid.densityValue[2], grid.densityValue[6]);;
    }

    if (edgeTable[cubeindex] & 2048)
    {
        vertlist[11] = VertexInterp(isolevel, grid.point[3], grid.point[7], grid.densityValue[3], grid.densityValue[7]);
    }


    /* Create the triangles */
    int triangleCount = 0;

    for (int i = 0; triTable[cubeindex][i] != -1; i += 3) 
    {
        Triangle t;
        t.p[0] = vertlist[triTable[cubeindex][i]];
        t.p[1] = vertlist[triTable[cubeindex][i + 1]];
        t.p[2] = vertlist[triTable[cubeindex][i + 2]];
        triangles.push_back(t);
        triangleCount++;
    }

    return(triangleCount);
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

double MarchingCubes::sphere(DirectX::SimpleMath::Vector3 p)
{
    return (p.x * p.x) + (p.y * p.y) + (p.z * p.z) - 1;
}

