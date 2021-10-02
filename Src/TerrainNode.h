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
        AudioEngine* audioEngine,
        ID3D11RenderTargetView ** bloomRenderTarget
    );

    void Draw(DX::DeviceResources& deviceResources, DirectX::SimpleMath::Matrix accumulatedTransform) override;
private:
    void CreateChests(DirectX::AudioEngine * &audioEngine, DX::DeviceResources & deviceResources);
private:

    int m_height = 1;
    int m_width = 10;
    int m_length = 10;
    float m_scale = 0.3f;

    std::vector<std::unique_ptr<Mesh>>              m_meshes;
    std::unique_ptr<PlayerNode>                     m_playerNode;

    std::unique_ptr<DirectX::SoundEffect>           m_coinsSFX;

    Light*          m_light;
    PlayerCamera*   m_playerCamera;

    float           m_isoLevel = 0.0f;
    float           m_fogDistance = 20.0f;
    int             m_dimention = 65;

    float m_lightPosX = 0.0f;
    float m_lightPosY = 10.0f;
    float m_lightPosZ = 0.0f;
    float m_lightDirX = 0.0f;
    float m_lightDirY = -1.0f;
    float m_lightDirZ = 0.0f;

    MeshObject m_chestModel;
    MeshObject m_coinsModel;
};

