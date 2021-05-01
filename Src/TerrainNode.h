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
        Camera* activeCamera,
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

    float m_isoLevel = 0.0f;
    float m_fogDistance = 20.0f;
    int m_dimention = 65;

    Light* m_light;
    PlayerCamera* m_playerCamera;

    float m_lightPosX = 0.0f;
    float m_lightPosY = 10.0f;
    float m_lightPosZ = 0.0f;
    float m_lightDirX = 0.0f;
    float m_lightDirY = -1.0f;
    float m_lightDirZ = 0.0f;
};

