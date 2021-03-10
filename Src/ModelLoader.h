#pragma once
#include <vector>
#include "CollisionDetection.h"
#include "Types.h"

class ModelLoader
{
public:
	MeshObject LoadModel(const char * fileName);
    MeshObject LoadModelFromTextFile(const char * fileName);
    MeshObject CreateSphere(float size);
    MeshObject CreateTerrain(int width, int height, HeightMapType* m_heightMap, float scale = 1, MeshPattern meshPattern = Uniform);
    void AddVertex(HeightMapType * m_heightMap, int index3, MeshObject &mesh, int &indexCount);
    void AddTerrainVertex(HeightMapType * m_heightMap, int index3, MeshObject &mesh, int index);
private:
    CollisionDetection m_collider;
};

