#include "pch.h"
#include "MainScene.h"
#include "ModelLoader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "InputLayout.h"
#include "VertexConstantBuffer.h"
#include "PixelConstantBuffer.h"
#include "Terrain.h"
#include "PlayerCamera.h"
#include "BoxCollider.h"
#include "TerrainNode.h"
#include "MarchingCubes.h"
#include "SimplexFunction.h"
#include "MarchingCubesMesh.h"
#include "MarchingCubesGeometryShader.h"
#include "Chest.h"
#include "FogConstantBuffer.h"

using namespace DirectX::SimpleMath;

MainScene::MainScene()
{
}

void MainScene::Initialise(DX::DeviceResources & deviceResources, PlayerCamera* playerCamera, Camera* activeCamera, ViewingFrustum* frustum)
{
    m_playerCamera = playerCamera;

    m_collisionDetector = std::make_unique<Collision>();

    // create the scene light
    auto lightPosition = Vector3(30.0f, 50.0f, -70.0f);

    m_light = std::make_unique<Light>();
    m_light->setAmbientColour(0.1f, 0.1f, 0.1f, 1.0f);
    m_light->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
    m_light->setPosition(lightPosition.x, lightPosition.y, lightPosition.z);
    m_light->setDirection(0.0f, 0.0f, 0.0f);

    m_sunLight = std::make_unique<Light>();
    m_sunLight->setAmbientColour(1.0f, 1.0f, 1.0f, 1.0f);
    m_sunLight->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
    m_sunLight->setSpecularColour(1.0f, 1.0f, 1.0f, 1.0f);
    m_sunLight->setSpecularPower(5.0f);
    m_sunLight->setPosition(lightPosition.x, lightPosition.y, lightPosition.z);
    m_sunLight->setDirection(1.0f, 1.0f, 1.0f);

    m_terrainWidth = 500;
    m_terrainHeight = 500;

    m_terrainTransform = Matrix::CreateScale(0.1) * Matrix::CreateTranslation(-10.0f, 0.0f, 0.0f);

    // create the models

    auto crateScale = Matrix::CreateScale(0.02);

    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(0), Vector3(5.0f, 0.1f, 5.0f), m_light.get(), m_playerCamera));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(3.14159), Vector3(27.7f, 0.1f, 5.8f), m_light.get(), m_playerCamera));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(1.5), Vector3(36.1f, 0.1f, 37.2f), m_light.get(), m_playerCamera));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(1.3),Vector3(63.2f, -0.5f, 59.4f), m_light.get(), m_playerCamera));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(1.3), Vector3(37.3f, 0.1f, 94.3f), m_light.get(), m_playerCamera));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(1), Vector3(89.6f, -0.5f, 125.57f), m_light.get(), m_playerCamera));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(1), Vector3(13.71f, -0.2f, 121.48f), m_light.get(), m_playerCamera));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(1), Vector3(18.81f, -0.2f, 130.57f), m_light.get(), m_playerCamera));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(-1), Vector3(40.48f, -0.2f, 182.99f), m_light.get(), m_playerCamera));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(-1), Vector3(14.9f, -0.2f, 149.25), m_light.get(), m_playerCamera));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(1.5), Vector3(143.13f, 0.1f, 185.27f), m_light.get(), m_playerCamera));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(4), Vector3(185.76, -0.2f, 68.31f), m_light.get(), m_playerCamera));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(0), Vector3(186.15, -0.2f, 41.73f), m_light.get(), m_playerCamera));
    m_meshes.push_back(std::make_unique<Chest>(deviceResources, crateScale, Matrix::CreateRotationY(-2), Vector3(118.34, -0.1f, 25.07f), m_light.get(), m_playerCamera));


    // store the model mesh pointers in a vector
    std::vector<Mesh*> meshPointers;

    for (auto &mesh : m_meshes)
    {
        meshPointers.push_back(mesh.get());
    }

    // create the root node
    m_rootNode = std::make_unique<SceneNode>(std::move(meshPointers), Matrix::Identity);


    m_terrainNode = std::make_unique<TerrainNode>(deviceResources, m_terrainTransform, m_light.get(), m_playerCamera, activeCamera, frustum, m_collisionDetector.get());

    m_rootNode->AddChild(std::move(m_terrainNode));


    // create the effects
    //m_skyBox = std::make_unique<SkyBox>(deviceResources);
    //m_bloom = std::make_unique<BloomPostProcessEffect>(deviceResources);
    //m_lensFlare = std::make_unique<LensFlareEffect>(deviceResources, playerCamera, m_light.get());

}

void MainScene::Draw(DX::DeviceResources & deviceResources, const DX::StepTimer& timer) const
{
   //m_bloom->SetSceneRenderTarget(deviceResources);

    if (ImGui::Begin("Output"))
    {
        ImGui::Text("Chests Found: %d", m_playerCamera->treasureChestsFound());
        ImGui::Text("Camera Position = %f, %f, %f", m_playerCamera->getPosition().x, m_playerCamera->getPosition().y, m_playerCamera->getPosition().z);
    }

    ImGui::End();

   //m_skyBox->Draw(deviceResources);
   m_rootNode->Draw(deviceResources, m_transform);


   //m_bloom->Bind(deviceResources);

   //m_lensFlare->Draw(deviceResources);
}

std::unique_ptr<Mesh> MainScene::ParseMesh(
    DX::DeviceResources & deviceResources,
    const MeshObject & modelMesh,
    Light * sceneLight,
    Matrix transform,
    const wchar_t * textureFileName,
    const wchar_t * vertexShaderFileName,
    const wchar_t * pixelShaderFileName,
    D3D_PRIMITIVE_TOPOLOGY topology
)
{
    std::vector<std::unique_ptr<Bindable>> bindables;

    // Create the vertex buffer
    bindables.push_back(std::make_unique<VertexBuffer<DirectX::VertexPositionNormalTexture>>(deviceResources, modelMesh.verticies));

    // Create the index buffer
    bindables.push_back(std::make_unique<IndexBuffer>(deviceResources, modelMesh.indices));

    // Create the vertex shader
    auto vertexShader = std::make_unique<VertexShader>(deviceResources, vertexShaderFileName);
    auto vertexShaderByteCode = vertexShader->GetBytecode();
    bindables.push_back(std::move(vertexShader));

    // Create the pixel shader
    bindables.push_back(std::make_unique<PixelShader>(deviceResources, pixelShaderFileName));

    bindables.push_back(std::make_unique<Sampler>(deviceResources, D3D11_TEXTURE_ADDRESS_WRAP));

    std::vector<D3D11_INPUT_ELEMENT_DESC> layout(std::begin(VertexPositionNormalTexture::InputElements), std::end(VertexPositionNormalTexture::InputElements));

    // Create the input layout
    bindables.push_back(std::make_unique<InputLayout>(deviceResources, layout, vertexShaderByteCode));

    // Create the rasterizer state
    bindables.push_back(std::make_unique<RasterizerState>(deviceResources, D3D11_CULL_BACK)); //D3D11_FILL_WIREFRAME

    // create and return the mesh
    return std::make_unique<Mesh>(deviceResources, std::move(bindables), sceneLight, textureFileName, transform, topology);
}

std::unique_ptr<SceneNode> MainScene::ParseNode()
{
    return std::unique_ptr<SceneNode>();
}
