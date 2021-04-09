#include "pch.h"
#include "MarchingCubesConstantBufferVS.h"

using namespace DirectX::SimpleMath;

MarchingCubesConstantBufferVS::MarchingCubesConstantBufferVS(DX::DeviceResources & deviceResources, const GameObject & parent, const float &cubeDimention)
    : m_parent(parent), m_cubeDimention(cubeDimention)
{
    if (!m_vertexConstantBuffer)
    {
        m_vertexConstantBuffer = std::make_unique<VertexConstantBuffer<MatrixBufferType>>(deviceResources);
    }
}

void MarchingCubesConstantBufferVS::Bind(DX::DeviceResources & deviceResources) noexcept
{
    auto cubePosition = m_parent.GetTransform();

    Vector3 cubeCentre = Vector3::Transform(Vector3(m_cubeDimention / 2.0f, m_cubeDimention / 2.0f, 0), cubePosition);

    Vector3 viewPosition = Vector3(cubeCentre.x, cubeCentre.y, cubeCentre.z + 1.0f);

    Vector3 unitZ = Vector3::UnitZ;

    Vector3 lookat = viewPosition - unitZ;

    auto viewMatrix = Matrix::CreateLookAt(viewPosition, lookat, Vector3::UnitY);

    auto projMatrix = Matrix::CreateOrthographic(m_cubeDimention, m_cubeDimention, 0.0f, 10.0f);

    const MatrixBufferType matrixBuffer =
    {
        DirectX::XMMatrixTranspose(m_parent.GetTransform()),
        DirectX::XMMatrixTranspose(viewMatrix),
        DirectX::XMMatrixTranspose(projMatrix)
    };

    m_vertexConstantBuffer->Update(deviceResources, matrixBuffer);
    m_vertexConstantBuffer->Bind(deviceResources);
}
