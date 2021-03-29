#include "pch.h"
#include "TerrainVertexBuffer.h"

TerrainVertexBuffer::TerrainVertexBuffer(DX::DeviceResources& deviceResources, const MeshObject& mesh)
    : m_mesh(mesh)
{
    m_terrainBuffer = std::make_shared<VertexBuffer<DirectX::VertexPositionNormalTexture>>(deviceResources, mesh.verticies);
}

void TerrainVertexBuffer::Bind(DX::DeviceResources & deviceResources) noexcept
{
    m_terrainBuffer->Bind(deviceResources);
}

void TerrainVertexBuffer::Update(DX::DeviceResources & deviceResources)
{
    m_terrainBuffer->Update(deviceResources, m_mesh.verticies);
}
