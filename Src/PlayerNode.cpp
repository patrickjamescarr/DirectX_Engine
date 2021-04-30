#include "pch.h"
#include "PlayerNode.h"
#include "BoxCollider.h"
#include "TerrainCollider.h"

using namespace DirectX::SimpleMath;

PlayerNode::PlayerNode(DX::DeviceResources & deviceResources, PlayerCamera * playerCamera, Collision* collision)
    : SceneNode(Matrix::Identity), m_playerCamera(playerCamera), m_collision(collision)
{
    m_transform = Matrix::CreateTranslation(m_playerCamera->getPosition());

    auto playerCollider = std::make_unique<BoxCollider>(deviceResources, Matrix::Identity, Vector3(1.f, 1.f, 1.f));

    m_collision->player_collider = playerCollider.get();

    //playerCamera->setCollision(m_collision);

    m_meshes.push_back(std::move(playerCollider));

    m_meshes.push_back(std::make_unique<TerrainCollider>(deviceResources, Matrix::Identity, playerCamera));

    // Add mesh pointers to the base mesh collection
    for (auto &mesh : m_meshes)
    {
        AddMesh(mesh.get());
    }
}

void PlayerNode::Draw(DX::DeviceResources & deviceResources, DirectX::SimpleMath::Matrix accumulatedTransform)
{
    Vector2 collisionPoint;

    m_transform = Matrix::CreateTranslation(m_playerCamera->getPosition());

    // Would need to make this position relative to the accumulated transform if the terrain were to move/rotate...
    SceneNode::Draw(deviceResources, Matrix::Identity);
}
