#pragma once
#include <vector>
#include "VertexTypes.h"

struct MeshObject {
    int vertexCount;
    int indexCount;
    std::vector<DirectX::VertexPositionNormalTexture> verticies;
    std::vector<uint32_t> indices;
    float scale = 1.f;
};

struct HeightMapType
{
    float x, y, z;
    float nx, ny, nz;
    float u, v;
};

enum MeshPattern
{
    Uniform = 0,
    Quilt = 1,
    Equilateral = 2
};