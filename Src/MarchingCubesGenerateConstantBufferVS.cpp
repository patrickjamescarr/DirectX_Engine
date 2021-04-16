#include "pch.h"
#include "MarchingCubesGenerateConstantBufferVS.h"

MarchingCubesGenerateConstantBufferVS::MarchingCubesGenerateConstantBufferVS(
    DX::DeviceResources & deviceResources, 
    const int * dimention, 
    const float * scale,
    UINT slot
)
    : m_scale(scale), m_dimention(dimention)
{
    if (!m_vertexConstantBuffer)
    {
        m_vertexConstantBuffer = std::make_unique<VertexConstantBuffer<CubeBufferType>>(deviceResources, slot);
    }
}

void MarchingCubesGenerateConstantBufferVS::Bind(DX::DeviceResources & deviceResources) noexcept
{
    const CubeBufferType matrixBuffer =
    {
        DirectX::SimpleMath::Vector4(*m_scale, *m_dimention, 0.0f, 0.0f)
    };

    m_vertexConstantBuffer->Update(deviceResources, matrixBuffer);
    m_vertexConstantBuffer->Bind(deviceResources);
}

