#include "pch.h"
#include "TerrainNode.h"
#include "Terrain.h"
#include "BoxCollider.h"
#include "MarchingCubesGeometryShader.h"
#include "Chest.h"

///
/// Terrain Node
///
///
///

using namespace DirectX::SimpleMath;

TerrainNode::TerrainNode(
    DX::DeviceResources& deviceResources, 
    const Matrix & transform, Light* light, 
    PlayerCamera* playerCamera, 
    Camera* activeCamera,
    ViewingFrustum* frustum,
    AudioEngine* audioEngine,
    ID3D11RenderTargetView ** bloomRenderTarget
)
    : SceneNode(transform), m_light(light), m_playerCamera(playerCamera)
{
    m_light->setPosition(m_lightPosX, m_lightPosY, m_lightPosZ);
    m_light->setDirection(m_lightDirX, m_lightDirY, m_lightDirZ);

    auto mcBlockOffset = (float)(m_dimention -1) * m_scale;
    auto halfBlock = mcBlockOffset / 2;

    // Create the marching cube terrain meshes
    for (int i = 0; i < m_width; i++)
    {
        for (int j = 0; j < m_length; j++)
        {
            for (int k = 0; k < m_height; k++)
            {
                auto cubeTransform =
                    Matrix::CreateScale(m_scale) *
                    Matrix::CreateTranslation(
                        halfBlock + (i * mcBlockOffset),
                        halfBlock + (k * mcBlockOffset),
                        halfBlock + (j * mcBlockOffset)
                    );

                //m_meshes.push_back(std::make_unique<BoxCollider>(deviceResources, cubeTransform, Vector3(m_dimention - 1, m_dimention - 1, m_dimention - 1)));

                m_meshes.push_back(std::make_unique<MarchingCubesGeometryShader>(
                    deviceResources, light, playerCamera, frustum, bloomRenderTarget,
                    Matrix::CreateTranslation(Vector3(mcBlockOffset * (float)i, mcBlockOffset * (float)k, mcBlockOffset * (float)j)),
                    i, k, j,
                    &m_isoLevel, &m_fogDistance, m_dimention, m_scale
                    ));
            }
        }
    }

    CreateChests(audioEngine, deviceResources);

    // Add mesh pointers to the base mesh collection
    for (auto &mesh : m_meshes)
    {
        AddMesh(mesh.get());
    }

    m_playerCamera->setBoundry(Vector3((float)m_width * mcBlockOffset, (float)m_height * mcBlockOffset, (float)m_length * mcBlockOffset));

    // add children 
    m_playerNode = std::make_unique<PlayerNode>(deviceResources, playerCamera);

    AddChild(std::move(m_playerNode));
}

void TerrainNode::CreateChests(DirectX::AudioEngine * &audioEngine, DX::DeviceResources & deviceResources)
{
    auto crateScale = Matrix::CreateScale(0.02);
    m_coinsSFX = std::make_unique<SoundEffect>(audioEngine, L"Sound\\coins.wav");

    // create the chests
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(0), Vector3(5.0f, 0.1f, 5.0f), m_light, m_playerCamera, m_coinsSFX.get(), m_fogDistance));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(3.14159), Vector3(25.6f, 0.1f, 12.6f), m_light, m_playerCamera, m_coinsSFX.get(), m_fogDistance));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(1.5), Vector3(42.66f, 0.1f, 45.23f), m_light, m_playerCamera, m_coinsSFX.get(), m_fogDistance));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(1.3), Vector3(69.01f, 3.62f, 58.92f), m_light, m_playerCamera, m_coinsSFX.get(), m_fogDistance));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(1.3), Vector3(39.96f, 0.1f, 84.02f), m_light, m_playerCamera, m_coinsSFX.get(), m_fogDistance));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(5), Vector3(94.65f, 0.2f, 110.55f), m_light, m_playerCamera, m_coinsSFX.get(), m_fogDistance));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(4), Vector3(2.71f, -0.2f, 103.75f), m_light, m_playerCamera, m_coinsSFX.get(), m_fogDistance));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(1), Vector3(23.f, -0.2f, 130.57f), m_light, m_playerCamera, m_coinsSFX.get(), m_fogDistance));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(-1), Vector3(49.5, -0.2f, 188.5f), m_light, m_playerCamera, m_coinsSFX.get(), m_fogDistance));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(-1), Vector3(15.2f, -0.2f, 152.5f), m_light, m_playerCamera, m_coinsSFX.get(), m_fogDistance));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(1.5), Vector3(143.13f, 0.1f, 185.27f), m_light, m_playerCamera, m_coinsSFX.get(), m_fogDistance));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(4), Vector3(169, 0.2f, 66), m_light, m_playerCamera, m_coinsSFX.get(), m_fogDistance));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(0), Vector3(129.31, 0.3f, 149.f), m_light, m_playerCamera, m_coinsSFX.get(), m_fogDistance));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(-2), Vector3(119.5, -0.1f, 139.12f), m_light, m_playerCamera, m_coinsSFX.get(), m_fogDistance));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(-2), Vector3(178.82, -0.1f, 98.79f), m_light, m_playerCamera, m_coinsSFX.get(), m_fogDistance));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(-2), Vector3(143.86, -0.1f, 51.91), m_light, m_playerCamera, m_coinsSFX.get(), m_fogDistance));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(5), Vector3(99.81, -0.1f, 50.99), m_light, m_playerCamera, m_coinsSFX.get(), m_fogDistance));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(5), Vector3(120.14, -0.1f, 34.61), m_light, m_playerCamera, m_coinsSFX.get(), m_fogDistance));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(5), Vector3(98.21, -0.1f, 2.82), m_light, m_playerCamera, m_coinsSFX.get(), m_fogDistance));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(5), Vector3(51.44, -0.1f, 5.84), m_light, m_playerCamera, m_coinsSFX.get(), m_fogDistance));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(0), Vector3(94.97, -0.1f, 98.08), m_light, m_playerCamera, m_coinsSFX.get(), m_fogDistance));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(-5), Vector3(56.39, -0.1f, 87.94), m_light, m_playerCamera, m_coinsSFX.get(), m_fogDistance));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(2.5), Vector3(143.5, 0.2f, 129.67), m_light, m_playerCamera, m_coinsSFX.get(), m_fogDistance));
}

void TerrainNode::Draw(DX::DeviceResources & deviceResources, DirectX::SimpleMath::Matrix accumulatedTransform)
{
    //ImGui::End();

    //if (ImGui::Begin("Marching Cubes"))
    //{
    //    ImGui::SliderFloat("Iso Level", &m_isoLevel, 0, 30.0f);
    //    ImGui::SliderFloat("Fog Distance", &m_fogDistance, 0, 100.0f);
    //}

    //ImGui::End();

    SceneNode::Draw(deviceResources, accumulatedTransform);
}
