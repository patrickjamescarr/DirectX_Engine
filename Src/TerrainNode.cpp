#include "pch.h"
#include "TerrainNode.h"
#include "Terrain.h"
#include "BoxCollider.h"
#include "MarchingCubesGeometryShader.h"

using namespace DirectX::SimpleMath;

TerrainNode::TerrainNode(
    DX::DeviceResources& deviceResources, 
    const Matrix & transform, Light* light, 
    PlayerCamera* playerCamera, 
    ViewingFrustum* frustum,
    Collision* collision
)
    : SceneNode(transform)
{
    auto terrainPosition = Matrix::CreateScale(m_scale) * Matrix::CreateTranslation(-10.0f, 0.0f, 0.0f);

    m_transform = Matrix::CreateTranslation(10.0f, 0.0f, 0.0f);

    //auto terrain = std::make_unique<Terrain>(deviceResources, light, L"Textures//seafloor.dds", terrainPosition, m_terrainWidth, m_terrainHeight, m_scale, playerCamera, frustum);

    //m_meshes.push_back(std::move(terrain));

    auto terrainColliderOffset = Matrix::CreateTranslation((m_terrainWidth * m_scale) / 2, 0.0f, (m_terrainHeight* m_scale) / 2);

    auto terrainCollider = std::make_unique<BoxCollider>(deviceResources, terrainPosition * terrainColliderOffset, Vector3(m_terrainWidth, 1.f, m_terrainHeight), m_scale);

    collision->terrain_collider = terrainCollider.get();

    m_meshes.push_back(std::move(terrainCollider));

    auto mcBlockOffset = (65.0f * 0.03f);
    auto mcBlockOffsetX = (65.0f * 0.03f);

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            m_meshes.push_back(std::make_unique<MarchingCubesGeometryShader>(deviceResources, light, playerCamera, Matrix::CreateTranslation(Vector3(-mcBlockOffsetX * (float)i, 0, mcBlockOffset * j)), i, j));
        }
    }

    // Add mesh pointers to the base mesh collection
    for (auto &mesh : m_meshes)
    {
        AddMesh(mesh.get());
    }

    // add children 

    m_playerNode = std::make_unique<PlayerNode>(deviceResources, playerCamera, collision);

    AddChild(std::move(m_playerNode));
}

void TerrainNode::Draw(DX::DeviceResources & deviceResources, DirectX::SimpleMath::Matrix accumulatedTransform)
{
    SceneNode::Draw(deviceResources, accumulatedTransform);
}
