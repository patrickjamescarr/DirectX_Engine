#pragma once
#include "Mesh.h"
#include "ModelLoader.h"
#include "PlayerCamera.h"
#include "ModelLoader.h"


class Chest :
    public Mesh
{
public:
    Chest(
        DX::DeviceResources& deviceResources,
        DirectX::SimpleMath::Matrix scale,
        DirectX::SimpleMath::Matrix rotation,
        DirectX::SimpleMath::Vector3 position,
        Light * sceneLight,
        PlayerCamera* playerCamera,
        SoundEffect* coinSoundFx,
        float fogDistance
    );
    void Draw(DX::DeviceResources& deviceResources, DirectX::FXMMATRIX accumulatedTransform) const override;
    void Update() override;
private:
    std::unique_ptr<Mesh> m_chestMesh;
    std::unique_ptr<Mesh> m_coinsMesh;

    bool m_coinsCollected = false;
    float m_fogEnd = 20.0f;
    DirectX::SimpleMath::Vector3 m_position;

    PlayerCamera * m_playerCamera;

    std::unique_ptr<SoundEffectInstance> m_coinFxInstance;
    SoundEffect* m_coinSoundFx;
};

