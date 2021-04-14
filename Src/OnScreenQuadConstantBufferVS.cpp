#include "pch.h"
#include "OnScreenQuadConstantBufferVS.h"

using namespace DirectX::SimpleMath;

OnScreenQuadConstantBufferVS::OnScreenQuadConstantBufferVS(DX::DeviceResources & deviceResources, const GameObject & parent, UINT slot)
    : m_parent(parent)
{
    if (!m_vertexConstantBuffer)
    {
        m_vertexConstantBuffer = std::make_unique<VertexConstantBuffer<MatrixBufferType>>(deviceResources, slot);
    }

    Vector3 eye(0.0f, 0.0f, 5.0f);
    Vector3 at(0.0f, 0.0f, 0.0f);

    m_view = Matrix::CreateLookAt(eye, at, Vector3::UnitY);
}

void OnScreenQuadConstantBufferVS::Bind(DX::DeviceResources & deviceResources) noexcept
{
    auto proj = Matrix::CreateOrthographic(5.f, 5.f, 1.f, 10.f);
    const MatrixBufferType matrixBuffer =
    {
        DirectX::XMMatrixTranspose(m_parent.GetTransform()),
        DirectX::XMMatrixTranspose(m_view),
        DirectX::XMMatrixTranspose(deviceResources.GetProjection())
    };

    m_vertexConstantBuffer->Update(deviceResources, matrixBuffer);
    m_vertexConstantBuffer->Bind(deviceResources);
}
