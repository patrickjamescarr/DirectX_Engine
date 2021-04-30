#pragma once
#include "Mesh.h"
#include "PlayerCamera.h"
#include "StreamOutput.h"

class TerrainCollider :
    public Mesh
{
private:
    struct GSOutput
    {
        DirectX::SimpleMath::Vector4 position;
        DirectX::SimpleMath::Vector4 camPosition;
        DirectX::SimpleMath::Vector4 densities1;
        DirectX::SimpleMath::Vector4 densities2;
    };
public:
    TerrainCollider(DX::DeviceResources& deviceResources, DirectX::SimpleMath::Matrix transform, PlayerCamera* camera);
    void Draw(DX::DeviceResources& deviceResources, DirectX::FXMMATRIX accumulatedTransform) const override;
private:
    PlayerCamera* m_camera;
    std::vector<std::unique_ptr<Bindable>> m_collisionPass;
    StreamOutput* m_streamOutput;
};

