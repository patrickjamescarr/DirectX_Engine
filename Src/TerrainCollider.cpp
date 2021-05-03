#include "pch.h"
#include "TerrainCollider.h"
#include "GeometryShader.h"
#include "InputLayout.h"
#include "Topology.h"
#include "DepthStencilState.h"
#include "RasterizerState.h"
#include "CameraConstantBuffer.h"
#include "SimplexNoise.h"
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "CameraConstantBuffer.h"

///
/// Terrain Collider
///
/// Enables collision detection by sampling the density function at a series of unit direction 
/// vectors relative to the player's current position. If the density is >= 0 in any given
/// direction then geometry is located there. Density values are sampled in the geometry shader
/// and passed back via stream output. The sampled density values for each potential direction
/// of movement are supplied to the player camera which performs the collision detection

using namespace DirectX::SimpleMath;

TerrainCollider::TerrainCollider(DX::DeviceResources & deviceResources, DirectX::SimpleMath::Matrix transform, PlayerCamera* camera)
    :Mesh(transform), m_camera(camera)
{
    // Send a single vertex to the vertex shader which will be updated with the player's current position via constant buffer
    VertexPosition position;
    position.position = Vector3::Zero;
    std::vector<VertexPosition> verts{ position };

    // create the vertex buffer
    m_collisionPass.push_back(std::make_unique<VertexBuffer<VertexPosition>>(deviceResources, verts));

    // point list topology, standard depth and stencil states
    m_collisionPass.push_back(std::make_unique<Topology>(deviceResources, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST));
    m_collisionPass.push_back(std::make_unique<DepthStencilState>(deviceResources));
    m_collisionPass.push_back(std::make_unique<RasterizerState>(deviceResources, D3D11_CULL_BACK));

    // constant buffer for sending the player position
    m_collisionPass.push_back(std::make_unique<CameraConstantBuffer>(deviceResources, camera));

    // create geometry shader with stream output
    m_collisionPass.push_back(std::make_unique<GeometryShader>(deviceResources, L"terrain_collision_gs.cso", true));

    // create the vertex shader
    auto vs = std::make_unique<VertexShader>(deviceResources, L"terrain_collision_vs.cso");

    // pull out the vertex shader bytecode
    auto vsBytecode = vs->GetBytecode();
    m_collisionPass.push_back(std::move(vs));

    // Create the vertex input layout description.
    std::vector<D3D11_INPUT_ELEMENT_DESC> layout(std::begin(VertexPosition::InputElements), std::end(VertexPosition::InputElements));

    // Create the input layout
    m_collisionPass.push_back(std::make_unique<InputLayout>(deviceResources, layout, vsBytecode));

    // stream output buffer size
    int bufferSize = sizeof(GSOutput);

    // create stream output stage to access collision data
    auto streamOutput = std::make_unique<StreamOutput>(deviceResources, bufferSize);

    // store a pointer to the SO buffer for access after the values have been calculated
    m_streamOutput = streamOutput.get();
    m_collisionPass.push_back(std::move(streamOutput));
}

void TerrainCollider::Draw(DX::DeviceResources & deviceResources, DirectX::FXMMATRIX accumulatedTransform) const
{
    m_accumulatedTransform = accumulatedTransform;

    // clear pixel shader as it's not being used
    deviceResources.GetD3DDeviceContext()->PSSetShader(NULL, 0, 0);

    for (auto& b : m_collisionPass)
    {
        b->Bind(deviceResources);
    }

    // perform the collision pass
    deviceResources.GetD3DDeviceContext()->DrawIndexed(1, 0, 0);

    // pull the calculated density values out from the geometry shader using stream output
    UpdateDensityValues(deviceResources);

    // clear the geometry shader
    deviceResources.GetD3DDeviceContext()->GSSetShader(0, 0, 0);
}

void TerrainCollider::UpdateDensityValues(DX::DeviceResources & deviceResources) const
{
    // pointer to stream output buffer 
    auto output = m_streamOutput->GetOutputBuffer();

    // extract the buffer description
    D3D11_BUFFER_DESC bufferDesc;
    output->GetDesc(&bufferDesc);

    // update the desc to use as a staging buffer with cpu read access
    bufferDesc.BindFlags = 0;
    bufferDesc.Usage = D3D11_USAGE_STAGING;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

    // create the staging buffer
    Microsoft::WRL::ComPtr<ID3D11Buffer> stagingBuffer;
    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateBuffer(&bufferDesc, nullptr, stagingBuffer.GetAddressOf()));

    // copy the SO buffer into the staging buffer so we can access the contents
    deviceResources.GetD3DDeviceContext()->CopyResource(stagingBuffer.Get(), output);

    // pull the density data out of the buffer
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    GSOutput densityData;
    deviceResources.GetD3DDeviceContext()->Map(stagingBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);
    densityData = *(GSOutput*)mappedResource.pData;

    m_camera->setDensities1(densityData.densities1);
    m_camera->setDensities2(densityData.densities2);
    m_camera->setDensities3(densityData.densities3);
    m_camera->setDensities4(densityData.densities4);
    m_camera->setDensities5(densityData.densities5);
    m_camera->setDensities6(densityData.densities6);
    m_camera->setDensities7(densityData.densities7);
}
