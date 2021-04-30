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

using namespace DirectX::SimpleMath;

TerrainCollider::TerrainCollider(DX::DeviceResources & deviceResources, DirectX::SimpleMath::Matrix transform, PlayerCamera* camera)
    :Mesh(transform), m_camera(camera)
{
    VertexPosition position;
    position.position = Vector3::Zero;
    std::vector<VertexPosition> verts{ position };

    m_collisionPass.push_back(std::make_unique<Topology>(deviceResources, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST));
    m_collisionPass.push_back(std::make_unique<DepthStencilState>(deviceResources, DepthStencilState::default));
    m_collisionPass.push_back(std::make_unique<RasterizerState>(deviceResources, D3D11_CULL_BACK));
    m_collisionPass.push_back(std::make_unique<VertexBuffer<VertexPosition>>(deviceResources, verts));
    m_collisionPass.push_back(std::make_unique<CameraConstantBuffer>(deviceResources, camera));

    // geometry shader
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
    m_streamOutput = streamOutput.get();
    m_collisionPass.push_back(std::move(streamOutput));
}

void TerrainCollider::Draw(DX::DeviceResources & deviceResources, DirectX::FXMMATRIX accumulatedTransform) const
{
    m_accumulatedTransform = accumulatedTransform;

    // set pixel shader to null
    deviceResources.GetD3DDeviceContext()->PSSetShader(0, 0, 0);

    for (auto& b : m_collisionPass)
    {
        b->Bind(deviceResources);
    }

    deviceResources.GetD3DDeviceContext()->DrawIndexed(1, 0, 0);

    auto output = m_streamOutput->GetOutputBuffer();

    D3D11_BUFFER_DESC bufferDesc;

    output->GetDesc(&bufferDesc);

    bufferDesc.BindFlags = 0;
    bufferDesc.Usage = D3D11_USAGE_STAGING;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

    Microsoft::WRL::ComPtr<ID3D11Buffer> stagingBuffer;

    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateBuffer(&bufferDesc, nullptr, stagingBuffer.GetAddressOf()));

    deviceResources.GetD3DDeviceContext()->CopyResource(stagingBuffer.Get(), output);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    GSOutput vertexData;

    deviceResources.GetD3DDeviceContext()->Map(stagingBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);

    vertexData = *(GSOutput*)mappedResource.pData;

    deviceResources.GetD3DDeviceContext()->GSSetShader(0, 0, 0);

    m_camera->setDensity(vertexData.position.x);
    m_camera->setDensities1(vertexData.densities1);
    m_camera->setDensities2(vertexData.densities2);

    if (ImGui::Begin("Cube Collision"))
    {
        ImGui::Text("Density: %f", vertexData.position.w);

        //ImGui::Text("Outside terrain :%f", vertexData.position.y);

        ImGui::Text("Cam pos x:%f, y:%f, z:%f", vertexData.camPosition.x, vertexData.camPosition.y, vertexData.camPosition.z);

        ImGui::Text("Can move:");

        if (vertexData.densities1.x >= 0)
        {
            ImGui::Text("forward blocked");
        }
        if (vertexData.densities1.y >= 0)
        {
            ImGui::Text("forward left blocked");
        }
        if (vertexData.densities1.z >= 0)
        {
            ImGui::Text("left blocked");
        }
        if (vertexData.densities1.w >= 0)
        {
            ImGui::Text("back left blocked");
        }
        if (vertexData.densities2.x >= 0)
        {
            ImGui::Text("back blocked");
        }
        if (vertexData.densities2.y >= 0)
        {
            ImGui::Text("back right blocked");
        }
        if (vertexData.densities2.z >= 0)
        {
            ImGui::Text("right blocked");
        }
        if (vertexData.densities2.w >= 0)
        {
            ImGui::Text("forward right blocked");
        }

        
        //ImGui::Text("forward left: %f", vertexData.densities1.y);
        //ImGui::Text("left: %f", vertexData.densities1.z);
        //ImGui::Text("back left: %f", vertexData.densities1.w);
        //ImGui::Text("back: %f", vertexData.densities2.x);
        //ImGui::Text("back right: %f", vertexData.densities2.y);
        //ImGui::Text("right: %f", vertexData.densities2.z);
        //ImGui::Text("forward right: %f", vertexData.densities2.w);
    }

    ImGui::End();
}
