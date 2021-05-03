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

using namespace DirectX::SimpleMath;

MainScene::MainScene()
{
}

void MainScene::Initialise(DX::DeviceResources & deviceResources, PlayerCamera* playerCamera, Camera* activeCamera, ViewingFrustum* frustum, AudioEngine* audioEngine)
{
    m_playerCamera = playerCamera;

    // create the scene light
    auto lightPosition = Vector3(30.0f, 50.0f, -70.0f);

    m_light = std::make_unique<Light>();
    m_light->setAmbientColour(0.1f, 0.1f, 0.1f, 1.0f);
    m_light->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
    m_light->setPosition(lightPosition.x, lightPosition.y, lightPosition.z);
    m_light->setDirection(0.0f, 0.0f, 0.0f);

    m_terrainTransform = Matrix::CreateScale(0.1) * Matrix::CreateTranslation(-10.0f, 0.0f, 0.0f);

    // store the model mesh pointers in a vector
    std::vector<Mesh*> meshPointers;

    for (auto &mesh : m_meshes)
    {
        meshPointers.push_back(mesh.get());
    }

    // create the effects
    m_bloom = std::make_unique<BloomPostProcessEffect>(deviceResources);
    m_bloomRenderTarget = m_bloom->GetSceneRenderTarget();
    
    // create the root node
    m_rootNode = std::make_unique<SceneNode>(std::move(meshPointers), Matrix::Identity);

    // create the terrain node
    m_terrainNode = std::make_unique<TerrainNode>(deviceResources, m_terrainTransform, m_light.get(), m_playerCamera, activeCamera, frustum, audioEngine, m_bloomRenderTarget);

    m_rootNode->AddChild(std::move(m_terrainNode));
}

void MainScene::Draw(DX::DeviceResources & deviceResources, const DX::StepTimer& timer) const
{
   m_bloom->SetSceneRenderTarget(deviceResources);

   m_rootNode->Draw(deviceResources, m_transform);

   m_bloom->Bind(deviceResources);
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
