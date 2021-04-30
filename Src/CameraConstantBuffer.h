#pragma once
#include "Bindable.h"
#include "Camera.h"
#include "VertexConstantBuffer.h"

class CameraConstantBuffer :
    public Bindable
{
private:
    struct CamBuffer
    {
        DirectX::SimpleMath::Vector4 forward;
        DirectX::SimpleMath::Vector4 right;
        DirectX::SimpleMath::Vector4 up;
    };
public:
    CameraConstantBuffer(DX::DeviceResources& deviceResources, Camera* activeCamera, UINT slot = 0);
    void Bind(DX::DeviceResources& deviceResources) noexcept override;
private:
    std::unique_ptr<VertexConstantBuffer<CamBuffer>> m_cameraBuffer;
    Camera* m_activeCamera;
};

