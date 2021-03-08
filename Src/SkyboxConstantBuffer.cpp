#include "pch.h"
#include "SkyboxConstantBuffer.h"

SkyboxConstantBuffer::SkyboxConstantBuffer(DX::DeviceResources & deviceResources)
{
    m_vertexConstantBuffer = std::make_unique<VertexConstantBuffer<MatrixBufferType>>(deviceResources);
}

void SkyboxConstantBuffer::Bind(DX::DeviceResources & deviceResources) noexcept
{
    const MatrixBufferType matrixBuffer =
    {
        // translate the skybox so that it is centered around the active camera 
        DirectX::XMMatrixTranspose(deviceResources.GetView() * deviceResources.GetProjection())
    };

    m_vertexConstantBuffer->Update(deviceResources, matrixBuffer);
    m_vertexConstantBuffer->Bind(deviceResources);
}
