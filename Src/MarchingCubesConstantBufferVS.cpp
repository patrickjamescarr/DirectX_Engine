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
    auto world = Matrix::Identity;

    auto width = deviceResources.GetBackBufferWidth();
    auto height = deviceResources.GetBackBufferHeight();

    float offsetX = ((float)width / 2.0f) - (65.0f / 2.0f);
    float offsetY = ((float)height / 2.0f) - (65.0f / 2.0f);

    //Set the View matrix
    auto view = Matrix::CreateLookAt(Vector3(offsetX, -offsetY, 100.0f), Vector3(offsetX, -offsetY, 0.0f), Vector3::UnitY);

    // Build an orthographic projection matrix
    auto projection = Matrix::CreateOrthographic(width, height, 1.0f, 1000.0f);

    const MatrixBufferType matrixBuffer =
    {
        DirectX::XMMatrixTranspose(world),
        DirectX::XMMatrixTranspose(view),
        DirectX::XMMatrixTranspose(projection)
    };

    m_vertexConstantBuffer->Update(deviceResources, matrixBuffer);
    m_vertexConstantBuffer->Bind(deviceResources);
}
