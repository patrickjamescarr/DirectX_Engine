#include "pch.h"
#include "Chest.h"
#include "VertexBuffer.h"
#include "ModelLoader.h"
#include "FogConstantBuffer.h"
#include "Types.h"

Chest::Chest(DX::DeviceResources & deviceResources, DirectX::SimpleMath::Matrix transform, Light * sceneLight, const Camera* camera)
    : Mesh(transform)
{
    ModelLoader modelLoader;

    auto scale = DirectX::SimpleMath::Matrix::CreateScale(0.2);

    // create chest mesh
    m_chestMesh = std::make_unique<Mesh>(deviceResources, sceneLight, scale * transform, L"Textures//chest_simple.dds", L"fog_vs.cso", L"fog_ps.cso", D3D11_CULL_FRONT);

    // load the chest model
    auto chestModel = modelLoader.LoadModel("Models//chest.obj");

    // create the vertex buffer for the chest
    m_chestMesh->AddBind(std::make_unique<VertexBuffer<DirectX::VertexPositionNormalTexture>>(deviceResources, chestModel.verticies));
    m_chestMesh->AddBind(std::make_unique<FogConstantBuffer>(deviceResources, &fogEnd, camera, ShaderType::Vertex, 1));

    // Create the index buffer for the chest
    m_chestMesh->AddIndexBuffer(std::make_unique<IndexBuffer>(deviceResources, chestModel.indices));


    // create coins mesh
    m_coinsMesh = std::make_unique<Mesh>(deviceResources, sceneLight, scale * transform, L"Textures//coins.dds", L"fog_vs.cso", L"fog_ps.cso", D3D11_CULL_FRONT);

    // load the coins model
    auto coinsModel = modelLoader.LoadModel("Models//coins.obj");

    // create the vertex buffer and store a pointer to it
    m_coinsMesh->AddBind(std::move(std::make_unique<VertexBuffer<DirectX::VertexPositionNormalTexture>>(deviceResources, coinsModel.verticies)));
    m_coinsMesh->AddBind(std::make_unique<FogConstantBuffer>(deviceResources, &fogEnd, camera, ShaderType::Vertex, 1));

    // Create the index buffer and store a pointer to it
    m_coinsMesh->AddIndexBuffer(std::make_unique<IndexBuffer>(deviceResources, coinsModel.indices));
}

void Chest::Draw(DX::DeviceResources & deviceResources, DirectX::FXMMATRIX accumulatedTransform) const
{
    m_accumulatedTransform = accumulatedTransform;

    m_chestMesh->Draw(deviceResources, accumulatedTransform);

    if (!m_coinsCollected)
    {
        m_coinsMesh->Draw(deviceResources, accumulatedTransform);
    }
}
