#pragma once
#include "Mesh.h"
#include "ModelLoader.h"
#include "Camera.h"

class Chest :
    public Mesh
{
public:
    Chest(DX::DeviceResources& deviceResources, DirectX::SimpleMath::Matrix transform, Light * sceneLight, const Camera* camera);
    void Draw(DX::DeviceResources& deviceResources, DirectX::FXMMATRIX accumulatedTransform) const override;
private:
    std::unique_ptr<Mesh> m_chestMesh;
    std::unique_ptr<Mesh> m_coinsMesh;

    bool m_coinsCollected = false;

    float fogEnd = 10.0f;
};

