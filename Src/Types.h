#pragma once
#include "pch.h"
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

struct CollisionIntersection
{
    float t, u, v;
};

enum ShaderType
{
    Vertex = 0,
    Geometry = 1,
    Pixel = 2
};