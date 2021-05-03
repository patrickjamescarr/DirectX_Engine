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
        DirectX::SimpleMath::Vector4 densities1;
        DirectX::SimpleMath::Vector4 densities2;
        DirectX::SimpleMath::Vector4 densities3;
        DirectX::SimpleMath::Vector4 densities4;
        DirectX::SimpleMath::Vector4 densities5;
        DirectX::SimpleMath::Vector4 densities6;
        DirectX::SimpleMath::Vector4 densities7;
    };
public:
    TerrainCollider(DX::DeviceResources& deviceResources, DirectX::SimpleMath::Matrix transform, PlayerCamera* camera);
    void Draw(DX::DeviceResources& deviceResources, DirectX::FXMMATRIX accumulatedTransform) const override;
private:
    void UpdateDensityValues(DX::DeviceResources & deviceResources) const;
private:
    PlayerCamera* m_camera;
    std::vector<std::unique_ptr<Bindable>> m_collisionPass;
    StreamOutput* m_streamOutput;
};

