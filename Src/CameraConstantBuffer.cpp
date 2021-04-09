#include "pch.h"
#include "CameraConstantBuffer.h"

CameraConstantBuffer::CameraConstantBuffer(DX::DeviceResources & deviceResources, Camera * activeCamera, UINT slot)
    : m_activeCamera(activeCamera)
{
    m_cameraBuffer = std::make_unique<VertexConstantBuffer<CamBuffer>>(deviceResources, slot);
}

void CameraConstantBuffer::Bind(DX::DeviceResources & deviceResources) noexcept
{
    const CamBuffer camBuffer =
    {
       m_activeCamera->getPosition()
    };

    m_cameraBuffer->Update(deviceResources, camBuffer);
    m_cameraBuffer->Bind(deviceResources);
}
