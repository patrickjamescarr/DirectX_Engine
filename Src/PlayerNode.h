#pragma once
#include "SceneNode.h"
#include "Light.h"
#include "PlayerCamera.h"
#include "Collision.h"

class PlayerNode
    : public SceneNode
{
public:
    PlayerNode(DX::DeviceResources& deviceResources, PlayerCamera* playerCamera, Collision* collision);
    void Draw(DX::DeviceResources& deviceResources, DirectX::SimpleMath::Matrix accumulatedTransform) override;
private :
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    PlayerCamera* m_playerCamera;
    Collision* m_collision;
};

