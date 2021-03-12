#pragma once
#include <vector>
#include "Types.h"
#include "MarchingCubesHeader.h"

class MarchingCubes
{
public:
    MeshObject GenerateTerrain(int xSize, int ySize, int zSize);
 
private:
    int Polygonise(GridCell grid, double isolevel, std::vector<Triangle>& triangles);
    void AddMeshVertex(DirectX::SimpleMath::Vector3 vertex, DirectX::SimpleMath::Vector3 normal, MeshObject &mesh, int &indexCount);

    DirectX::SimpleMath::Vector3 VertexInterp(
        double isolevel,
        DirectX::SimpleMath::Vector3 p1,
        DirectX::SimpleMath::Vector3 p2,
        double valp1,
        double valp2
    );
};

