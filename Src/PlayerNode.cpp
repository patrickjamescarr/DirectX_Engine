#include "pch.h"
#include "PlayerNode.h"
#include "BoxCollider.h"
#include "TerrainCollider.h"

///
/// Player Node
///
///
///

using namespace DirectX::SimpleMath;

PlayerNode::PlayerNode(DX::DeviceResources & deviceResources, PlayerCamera * playerCamera)
    : SceneNode(Matrix::Identity), m_playerCamera(playerCamera)
{
    m_transform = Matrix::CreateTranslation(m_playerCamera->getPosition());

    m_meshes.push_back(std::make_unique<TerrainCollider>(deviceResources, Matrix::Identity, playerCamera));

    // Add mesh pointers to the base mesh collection
    for (auto &mesh : m_meshes)
    {
        AddMesh(mesh.get());
    }
}

void PlayerNode::Draw(DX::DeviceResources & deviceResources, const DirectX::SimpleMath::Matrix& accumulatedTransform)
{
    m_transform = Matrix::CreateTranslation(m_playerCamera->getPosition());

    SceneNode::Draw(deviceResources, accumulatedTransform);
}
