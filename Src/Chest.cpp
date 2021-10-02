#include "pch.h"
#include "Chest.h"
#include "VertexBuffer.h"
#include "FogConstantBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "Sampler.h"
#include "InputLayout.h"
#include "RasterizerState.h"

using namespace DirectX::SimpleMath;

Chest::Chest(
    DX::DeviceResources & deviceResources, 
    DirectX::SimpleMath::Matrix scale, 
    DirectX::SimpleMath::Matrix rotation, 
    DirectX::SimpleMath::Vector3 position, 
    const MeshObject& chestModel,
    const MeshObject& coinsModel,
    Light * sceneLight, 
    PlayerCamera* playerCamera,
    SoundEffect* coinSoundFx,
    float fogDistance
)
    : Mesh(scale * rotation * Matrix::CreateTranslation(position)), 
    m_playerCamera(playerCamera), m_position(position), m_coinSoundFx(coinSoundFx), m_fogEnd(fogDistance)
{
    auto transform = scale * rotation * Matrix::CreateTranslation(position);

    // create and return the mesh
    m_chestMesh = std::make_unique<Mesh>(deviceResources,  sceneLight, transform, L"Textures//chest_simple.dds", L"fog_vs.cso", L"fog_ps.cso", D3D11_CULL_FRONT);

    m_chestMesh->AddBind(std::make_unique<VertexBuffer<DirectX::VertexPositionNormalTexture>>(deviceResources, chestModel.verticies));
    m_chestMesh->AddBind((std::make_unique<FogConstantBuffer>(deviceResources, &m_fogEnd, playerCamera, ShaderType::Vertex, 1)));
    // Create the index buffer
    m_chestMesh->AddIndexBuffer(std::make_unique<IndexBuffer>(deviceResources, chestModel.indices));

    // create and return the mesh
    m_coinsMesh = std::make_unique<Mesh>(deviceResources, sceneLight, transform, L"Textures//coins.dds", L"fog_vs.cso", L"fog_ps.cso", D3D11_CULL_FRONT);

    m_coinsMesh->AddBind(std::make_unique<VertexBuffer<DirectX::VertexPositionNormalTexture>>(deviceResources, coinsModel.verticies));
    m_coinsMesh->AddBind((std::make_unique<FogConstantBuffer>(deviceResources, &m_fogEnd, playerCamera, ShaderType::Vertex, 1)));
    // Create the index buffer
    m_coinsMesh->AddIndexBuffer(std::make_unique<IndexBuffer>(deviceResources, coinsModel.indices));

    m_coinFxInstance = coinSoundFx->CreateInstance();
}

void Chest::Draw(DX::DeviceResources & deviceResources, DirectX::FXMMATRIX accumulatedTransform) const
{
    auto playerPos = m_playerCamera->getPosition();

    auto distance = Vector3::Distance(playerPos, m_position);

    if (distance > 35)
    {
        return;
    }

    m_chestMesh->Draw(deviceResources, accumulatedTransform);

    if (!m_coinsCollected)
    {
        m_coinsMesh->Draw(deviceResources, accumulatedTransform);
    }
}

void Chest::Update()
{
    if (!m_coinsCollected)
    {
        auto playerPos = m_playerCamera->getPosition();

        auto distance = Vector3::Distance(playerPos, m_position);

        if (distance < 1.5)
        {
            m_coinsCollected = true;
            m_playerCamera->chestFound();
            m_coinFxInstance->Play();
        }
    }
}
