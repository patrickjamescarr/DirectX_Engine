#include "pch.h"
#include "MarchingCubesGeometryShader.h"
#include "GeometryShader.h"

using namespace DirectX::SimpleMath;

MarchingCubesGeometryShader::MarchingCubesGeometryShader(
    DX::DeviceResources & deviceResources, 
    Light * sceneLight, 
    DirectX::SimpleMath::Matrix transform
)
    : Mesh(
        deviceResources, 
        sceneLight, 
        transform, 
        L"Textures//sun2.dds",
        L"marching_cubes_terrain_vs.cso",
        L"marching_cubes_terrain_ps.cso",
        D3D11_PRIMITIVE_TOPOLOGY_POINTLIST
    )
{
    auto gs = std::make_unique<GeometryShader>(deviceResources, L"marching_cubes_terrain_gs.cso");
    AddBind(std::move(gs));

    std::vector<DirectX::VertexPositionNormalTexture> verts;
    std::vector<uint32_t> indices;

    VertexPositionNormalTexture v;

    v.position = Vector3(0, 0, 0);
    v.normal = Vector3(0, 0, 0);
    v.textureCoordinate = Vector2(0, 0);

    verts.push_back(v);

    indices.push_back(0);

    // create the vertex buffer and store a pointer to it
    auto vertexBuffer = std::make_unique<VertexBuffer>(deviceResources, verts);
    m_vertexBuffer = vertexBuffer.get();
    AddBind(std::move(vertexBuffer));

    // Create the index buffer and store a pointer to it
    auto indexBuffer = std::make_unique<IndexBuffer>(deviceResources, indices);
    m_indexBuffer = indexBuffer.get();
    AddIndexBuffer(std::move(indexBuffer));
}

void MarchingCubesGeometryShader::Draw(DX::DeviceResources & deviceResources, DirectX::FXMMATRIX accumulatedTransform) const noexcept
{
    Mesh::Draw(deviceResources, accumulatedTransform);

    // clear the geometry shader
    deviceResources.GetD3DDeviceContext()->GSSetShader(0, 0, 0);
}
