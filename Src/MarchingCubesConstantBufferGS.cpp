#include "pch.h"
#include "MarchingCubesConstantBufferGS.h"

MarchingCubesConstantBufferGS::MarchingCubesConstantBufferGS(
    DX::DeviceResources & deviceResources, 
    const GameObject & parent, 
    const float* isolevel, 
    const int* dimention
)
    : m_parent(parent), m_isoLevel(isolevel), m_dimention(dimention)
{   
    // matrix buffer in slot 0
    if (!m_matrixConstantBuffer)
    {
        m_matrixConstantBuffer = std::make_unique<GeometryConstantBuffer<MatrixBufferType>>(deviceResources);
    }

    // cube buffer in slot 1
    if (!m_cubeBuffer)
    {
        m_cubeBuffer = std::make_unique<GeometryConstantBuffer<CubeBuffer>>(deviceResources, 1);
    }

    // tri table buffer in slot 2
    if (!m_triTableBuffer)
    {
        m_triTableBuffer = std::make_unique<GeometryConstantBuffer<TriTableBuffer>>(deviceResources, 2);
    }

    for (int i = 0; i < 256; i++)
    {
        m_cubeBufferData.edgeTable[i].x = m_edgeTable[i];
    }

    for (int i = 0; i < 4096; i++)
    {
        m_triTableData.triTable[i].x = m_triTable[i];
    }
}

void MarchingCubesConstantBufferGS::Bind(DX::DeviceResources & deviceResources) noexcept
{
    const MatrixBufferType matrixBuffer =
    {
        DirectX::XMMatrixTranspose(m_parent.GetTransform()),
        DirectX::XMMatrixTranspose(deviceResources.GetView()),
        DirectX::XMMatrixTranspose(deviceResources.GetProjection())
    };

    m_matrixConstantBuffer->Update(deviceResources, matrixBuffer);
    m_matrixConstantBuffer->Bind(deviceResources);

    m_cubeBufferData.isoLevel = DirectX::SimpleMath::Vector4(*m_isoLevel, 0, 0, 0); //DirectX::SimpleMath::Vector4(*m_isoLevel, 0, 0, 0);
    m_cubeBufferData.dimention = DirectX::SimpleMath::Vector4(*m_dimention, 0, 0, 0);
    m_cubeBuffer->Update(deviceResources, m_cubeBufferData);
    m_cubeBuffer->Bind(deviceResources);

    m_triTableBuffer->Update(deviceResources, m_triTableData);
    m_triTableBuffer->Bind(deviceResources);
}
