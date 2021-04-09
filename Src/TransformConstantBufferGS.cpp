#include "pch.h"
#include "TransformConstantBufferGS.h"

TransformConstantBufferGS::TransformConstantBufferGS(DX::DeviceResources & deviceResources, const GameObject & parent)
    : m_parent(parent)
{
    if (!m_geometryConstantBuffer)
    {
        m_geometryConstantBuffer = std::make_unique<GeometryConstantBuffer<MatrixBufferType>>(deviceResources);
    }
}

void TransformConstantBufferGS::Bind(DX::DeviceResources & deviceResources) noexcept
{
    const MatrixBufferType matrixBuffer =
    {
        DirectX::XMMatrixTranspose(m_parent.GetTransform()),
        DirectX::XMMatrixTranspose(deviceResources.GetView()),
        DirectX::XMMatrixTranspose(deviceResources.GetProjection())
    };

    m_geometryConstantBuffer->Update(deviceResources, matrixBuffer);
    m_geometryConstantBuffer->Bind(deviceResources);
}
