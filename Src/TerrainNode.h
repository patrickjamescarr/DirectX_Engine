#pragma once
#include "SceneNode.h"
#include "Terrain.h"
#include "Light.h"
#include "PlayerCamera.h"
#include "PlayerNode.h"
#include "Collision.h"
#include "ViewingFrustum.h"

class TerrainNode :
    public SceneNode
{
public:
    TerrainNode(
        DX::DeviceResources& deviceResources, 
        const DirectX::SimpleMath::Matrix & transform, 
        Light* light, 
        PlayerCamera* playerCamera, 
        ViewingFrustum* frustum, 
        Collision* collision
    );
    void Draw(DX::DeviceResources& deviceResources, DirectX::SimpleMath::Matrix accumulatedTransform) override;
private:
    int m_terrainWidth = 128;
    int m_terrainHeight = 128;
    float m_scale = 0.1;

    std::vector<std::unique_ptr<Mesh>> m_meshes;

    std::unique_ptr<PlayerNode> m_playerNode;
};

