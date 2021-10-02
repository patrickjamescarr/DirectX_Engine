#pragma once
#include "Types.h"
#include "MarchingCubesHeader.h"
#include "DensityFunction.h"
#include <map>

class MarchingCubes
{
friend class MarchingCubesMesh;
public:
    MarchingCubes(DensityFunction* function);

    MeshObject Generate(int xSize, int ySize, int zSize);

    void SetFunction(DensityFunction* function) {
        m_function = function;
    }

    void DisplayUI();
private:
    int Polygonise(GridCell grid, double isolevel, std::vector<Triangle>& triangles);
    void AddMeshVertex(
        DirectX::SimpleMath::Vector3 vertex,
        MeshObject &mesh, 
        int &indexCount
    );
    std::vector<DirectX::SimpleMath::Vector3> GenerateCubeVerticies(int i, int j, int k);

    DirectX::SimpleMath::Vector3 VertexInterp(
        double isolevel,
        DirectX::SimpleMath::Vector3 p1,
        DirectX::SimpleMath::Vector3 p2,
        double valp1,
        double valp2
    );

    double sphere(DirectX::SimpleMath::Vector3);

    void CalculateGradient(const DirectX::SimpleMath::Vector3 &Pos, DirectX::SimpleMath::Vector3 &Normal);
private:
    DensityFunction* m_function;

    float m_scale = 0.03;
    float m_isoLevel = 0;
};

