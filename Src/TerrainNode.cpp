#include "pch.h"
#include "TerrainNode.h"
#include "Terrain.h"
#include "BoxCollider.h"
#include "MarchingCubesGeometryShader.h"

using namespace DirectX::SimpleMath;

TerrainNode::TerrainNode(
    DX::DeviceResources& deviceResources, 
    const Matrix & transform, Light* light, 
    PlayerCamera* playerCamera, 
    Camera* activeCamera,
    ViewingFrustum* frustum,
    Collision* collision,
    ID3D11RenderTargetView ** bloomRenderTarget
)
    : SceneNode(transform), m_light(light), m_playerCamera(playerCamera)
{
    auto terrainPosition = Matrix::CreateScale(m_scale) * Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);

    m_transform = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);

    float mcScale = 0.3f;

    auto mcBlockOffset = (float)(m_dimention -1) * mcScale;

    auto halfBlock = mcBlockOffset / 2;

    int height = 1;
    int width = 10;
    int length = 10;

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < length; j++)
        {
            for (int k = 0; k < height; k++)
            {
                auto cubeTransform =
                    Matrix::CreateScale(mcScale) *
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
                    &m_isoLevel, &m_fogDistance, m_dimention, mcScale
                    ));
            }
        }
    }

    // Add mesh pointers to the base mesh collection
    for (auto &mesh : m_meshes)
    {
        AddMesh(mesh.get());
    }

    m_playerCamera->setBoundry(Vector3((float)width * mcBlockOffset, (float)height * mcBlockOffset, (float)length * mcBlockOffset));

    // add children 

    m_playerNode = std::make_unique<PlayerNode>(deviceResources, playerCamera, collision);

    AddChild(std::move(m_playerNode));
}

void TerrainNode::Draw(DX::DeviceResources & deviceResources, DirectX::SimpleMath::Matrix accumulatedTransform)
{
    //if (ImGui::Begin("Light"))
    //{
    //    ImGui::SliderFloat("Position x", &m_lightPosX, -50.0f, 50.0f);
    //    ImGui::SliderFloat("Position y", &m_lightPosY, -50.0f, 50.0f);
    //    ImGui::SliderFloat("Position z", &m_lightPosZ, -50.0f, 50.0f);
    //    ImGui::SliderFloat("Direction x", &m_lightDirX, -50.0f, 50.0f);
    //    ImGui::SliderFloat("Direction y", &m_lightDirY, -50.0f, 50.0f);
    //    ImGui::SliderFloat("Direction z", &m_lightDirZ, -50.0f, 50.0f);
    //}

    m_light->setPosition(m_lightPosX, m_lightPosY, m_lightPosZ);
    m_light->setDirection(m_lightDirX, m_lightDirY, m_lightDirZ);

    //ImGui::End();

    //if (ImGui::Begin("Marching Cubes"))
    //{
    //    ImGui::SliderFloat("Iso Level", &m_isoLevel, 0, 30.0f);
    //    ImGui::SliderFloat("Fog Distance", &m_fogDistance, 0, 100.0f);
    //}

    //ImGui::End();

    SceneNode::Draw(deviceResources, accumulatedTransform);
}
