#pragma once
#include "SceneNode.h"
#include "Light.h"
#include "PlayerCamera.h"
#include "Collision.h"

class PlayerNode
    : public SceneNode
{
public:
    PlayerNode(DX::DeviceResources& deviceResources, PlayerCamera* playerCamera);
    void Draw(DX::DeviceResources& deviceResources, const DirectX::SimpleMath::Matrix& accumulatedTransform) override;
private :
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    PlayerCamera* m_playerCamera;

    int m_chestsFound = 0;
};

