#pragma once
#include "Bindable.h"
#include "Camera.h"
#include "VertexConstantBuffer.h"

class CameraConstantBuffer :
    public Bindable
{
private:
    __declspec(align(16)) // buffer byteWidth needs to be a multiple of 16
    struct CamBuffer
    {
        DirectX::SimpleMath::Vector3 camPosition;
    };
public:
    CameraConstantBuffer(DX::DeviceResources& deviceResources, Camera* activeCamera, UINT slot = 0);
    void Bind(DX::DeviceResources& deviceResources) noexcept override;
private:
    std::unique_ptr<VertexConstantBuffer<CamBuffer>> m_cameraBuffer;
    Camera* m_activeCamera;
};

