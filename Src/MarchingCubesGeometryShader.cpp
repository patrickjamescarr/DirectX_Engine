#include "pch.h"
#include "MarchingCubesGeometryShader.h"
#include "GeometryShader.h"
#include "InputLayout.h"
#include "Topology.h"
#include "DepthStencilState.h"
#include "TransformConstantBufferVS.h"
#include "MarchingCubesConstantBufferGS.h"
#include "RasterizerState.h"
#include "Sampler.h"
#include "CameraConstantBuffer.h"
#include "LightConstantBuffer.h"
#include "MarchingCubesConstantBufferVS.h";
#include "SimplexNoise.h"
#include "TerrainDensityFunction.h"
#include "MarchingCubesGenerateConstantBufferVS.h"
#include "Texture3D.h"
#include "FogConstantBuffer.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

MarchingCubesGeometryShader::MarchingCubesGeometryShader(
    DX::DeviceResources & deviceResources,
    Light * sceneLight,
    Camera* playerCamera,
    ViewingFrustum* viewingFrustum,
    SimpleMath::Matrix transform,
    int xPos,
    int yPos,
    int zPos,
    const float * isoLevel,
    const float * fogEnd,
    const int &dimention,
    float scale
)
    : Mesh(transform), m_isoLevel(isoLevel), m_dimention(dimention), m_frustum(viewingFrustum), 
    m_playerCamera(playerCamera), m_scale(scale), m_yPos(yPos), m_fogEnd(fogEnd), m_deviceResources(deviceResources)//, m_streamOutThread{}
{
    if (yPos == 0)
    {
        // create floor terrain
        m_floorTerrain = std::make_unique<HeightMapTerrain>(
            deviceResources,
            sceneLight, L"Textures\\sand.dds",
            Matrix::CreateScale(m_scale) * transform,
            dimention, dimention, scale,
            Vector3(xPos, 0, zPos),
            playerCamera, viewingFrustum, fogEnd
            );
    }

    // configure density volume render pass
    {
        float size = (float)(m_dimention) / 2.0f;
        float dimMinusOne = (float)m_dimention - 1.0f;
        float xIncrement = (float)xPos * dimMinusOne;
        float yIncrement = (float)yPos * dimMinusOne;
        float zIncrement = (float)zPos * dimMinusOne;

        auto scaledOffset = dimMinusOne * m_scale;
        auto halfBlock = scaledOffset / 2;

        m_viewingFrustumTransform =
            Matrix::CreateScale(m_scale) * Matrix::CreateTranslation(
                halfBlock + (xIncrement * m_scale),
                halfBlock + (yIncrement * m_scale),
                halfBlock + (zIncrement * m_scale)
            );

        m_position = Vector3(xPos, yPos, zPos);
        m_worldPosition = Vector3(xPos * scaledOffset, yPos * scaledOffset, zPos * scaledOffset);

        std::vector<VertexPositionTexture> verts;
        verts.push_back(VertexPositionTexture(Vector3(-size + xIncrement, size - yIncrement, zIncrement), Vector2(0, 0)));  // top left
        verts.push_back(VertexPositionTexture(Vector3(size + xIncrement, size - yIncrement, zIncrement), Vector2(0, 1)));   // top right
        verts.push_back(VertexPositionTexture(Vector3(size + xIncrement, -size - yIncrement, zIncrement), Vector2(1, 1)));  // bottom right

        verts.push_back(VertexPositionTexture(Vector3(size + xIncrement, -size - yIncrement, zIncrement), Vector2(1, 1)));  // bottom right
        verts.push_back(VertexPositionTexture(Vector3(-size + xIncrement, -size - yIncrement, zIncrement), Vector2(1, 0))); // bottom left
        verts.push_back(VertexPositionTexture(Vector3(-size + xIncrement, size - yIncrement, zIncrement), Vector2(0, 0)));  // top left

        m_densityVolumeRenderPass.push_back(std::make_unique<Topology>(deviceResources, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
        m_densityVolumeRenderPass.push_back(std::make_unique<DepthStencilState>(deviceResources, DepthStencilState::default));
        m_densityVolumeRenderPass.push_back(std::make_unique<MarchingCubesDensityConstantBufferVS>(deviceResources, transform, m_dimention, xPos, yPos));
        m_densityVolumeRenderPass.push_back(std::make_unique<RasterizerState>(deviceResources, D3D11_CULL_BACK));
        m_densityVolumeRenderPass.push_back(std::make_unique<VertexBuffer<VertexPositionTexture>>(deviceResources, verts));

        // 3D texture description for storing the density values
        CD3D11_TEXTURE3D_DESC densityVolTexDesc;
        densityVolTexDesc.Height = m_dimention;
        densityVolTexDesc.Width = m_dimention;
        densityVolTexDesc.Depth = m_dimention;
        densityVolTexDesc.MipLevels = 1;
        densityVolTexDesc.Format = DXGI_FORMAT_R16_FLOAT;
        densityVolTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        densityVolTexDesc.Usage = D3D11_USAGE_DEFAULT;
        densityVolTexDesc.CPUAccessFlags = 0;
        densityVolTexDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

        auto rt = std::make_unique<RenderTarget>(deviceResources, densityVolTexDesc);
        m_buildDensitiesRT = rt.get();
        m_densityVolumeRenderPass.push_back(std::move(rt));

        // create the vertex shader
        auto vs = std::make_unique<VertexShader>(deviceResources, L"mc_build_densities_vs.cso");

        // pull out the vertex shader bytecode
        auto vsBytecode = vs->GetBytecode();
        m_densityVolumeRenderPass.push_back(std::move(vs));

        // geometry shader
        m_densityVolumeRenderPass.push_back(std::make_unique<GeometryShader>(deviceResources, L"mc_build_densities_gs.cso"));

        // pixel shader
        m_densityVolumeRenderPass.push_back(std::make_unique<PixelShader>(deviceResources, L"mc_build_densities_ps.cso"));

        // Create the vertex input layout description.
        std::vector<D3D11_INPUT_ELEMENT_DESC> layout(std::begin(VertexPositionTexture::InputElements), std::end(VertexPositionTexture::InputElements));

        // Create the input layout
        m_densityVolumeRenderPass.push_back(std::make_unique<InputLayout>(deviceResources, layout, vsBytecode));
    }

    //m_quad = std::make_unique<OnScreenQuad>(deviceResources, 100, 100, Matrix::CreateTranslation(1.5, 1.5, 0), m_texture2dRT);

    // configure generate verticies render pass
    {
        std::vector<VertexPositionTexture> generationVerts;
        m_generateVertexCount = 0;

        float textureCoordinatesStep = 1.0f / ((float)m_dimention - 1.0f);


        std::vector<uint32_t> indices;
        int index = 0;

        for (int j = 0; j < (m_dimention - 1); j++)
        {
            for (int i = 0; i < (m_dimention - 1); i++)
            {
                VertexPositionTexture v;

                v.position = Vector3((float)i * m_scale, (float)j * m_scale, 1) ;
                v.textureCoordinate = Vector2((float)i * textureCoordinatesStep, (float)j * textureCoordinatesStep);
                generationVerts.push_back(v);

                m_generateVertexCount++;

                indices.push_back(index);
                index++;
            }
        }

        m_generateVertsRenderPass.push_back(std::make_unique<IndexBuffer>(deviceResources, indices));


        m_generateVertsRenderPass.push_back(std::make_unique<Topology>(deviceResources, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST));
        m_generateVertsRenderPass.push_back(std::make_unique<DepthStencilState>(deviceResources, DepthStencilState::default));
        m_generateVertsRenderPass.push_back(std::make_unique<RasterizerState>(deviceResources, D3D11_CULL_BACK)); //D3D11_FILL_WIREFRAME
        
        // buffers
        //vertex buffers
        m_generateVertsRenderPass.push_back(std::make_unique<MarchingCubesGenerateConstantBufferVS>(deviceResources, &m_dimention, &m_scale, 1));
        m_generateVertsRenderPass.push_back(std::make_unique<TransformConstantBufferVS>(deviceResources, *this));
        // geometry buffer
        m_generateVertsRenderPass.push_back(std::make_unique<MarchingCubesConstantBufferGS>(deviceResources, *this, m_isoLevel, &m_dimention));
        //pixel buffer
        m_generateVertsRenderPass.push_back(std::make_unique<LightConstantBuffer>(deviceResources, sceneLight));

        m_generateVertsRenderPass.push_back(std::make_unique<FogConstantBuffer>(deviceResources, m_fogEnd, playerCamera, ShaderType::Geometry, 3));

        // create the vertex buffer and store a pointer to it
        auto vertexBuffer = std::make_unique<VertexBuffer<VertexPositionTexture>>(deviceResources, generationVerts);
        m_generateVertsVertexBuffer = vertexBuffer.get();
        m_generateVertsRenderPass.push_back(std::move(vertexBuffer));

        m_gsSampler = std::make_unique<Sampler>(deviceResources, D3D11_TEXTURE_ADDRESS_CLAMP);

        // shaders
        m_generateVertsRenderPass.push_back(std::make_unique<GeometryShader>(deviceResources, L"mc_generate_vertices_gs.cso"));
        m_generateVertsRenderPass.push_back(std::make_unique<PixelShader>(deviceResources, L"mc_generate_vertices_ps.cso"));
        auto vs = std::make_unique<VertexShader>(deviceResources, L"mc_generate_vertices_vs.cso");
        auto vsBytecode = vs->GetBytecode();
        m_generateVertsRenderPass.push_back(std::move(vs));

        // Create the vertex input layout description.
        std::vector<D3D11_INPUT_ELEMENT_DESC> layout(std::begin(VertexPositionTexture::InputElements), std::end(VertexPositionTexture::InputElements));

        // Create the input layout
        m_generateVertsRenderPass.push_back(std::make_unique<InputLayout>(deviceResources, layout, vsBytecode));

        //int bufferSize = pow((m_dimention - 1), 3) * sizeof(GSOutput) * 15;;

        //// create stream output stage to access vertices created in the geometry shader
        //auto streamOutput = std::make_unique<StreamOutput>(deviceResources, bufferSize);
        //m_streamOutput = streamOutput.get();
        //m_generateVertsRenderPass.push_back(std::move(streamOutput));
    }
}

void MarchingCubesGeometryShader::Draw(
    DX::DeviceResources & deviceResources,
    FXMMATRIX accumulatedTransform
) const noexcept
{
    m_accumulatedTransform = accumulatedTransform;

    Vector3 pos = Vector3::Transform(m_position, m_accumulatedTransform * m_viewingFrustumTransform);

    auto cubeRadius = ((float)m_dimention / 2.0f) * m_scale;

    bool draw = m_frustum->CheckCube(pos.x, pos.y, pos.z, cubeRadius);

    //if (!draw)
    //{
    //    if (ImGui::Begin("Marching Cubes"))
    //    {
    //        ImGui::Text("Culling cube at: %f, %f", pos.x, pos.z);
    //    }

    //    ImGui::End();

    //    return;
    //}

    auto playerPos = m_playerCamera->getPosition(); /// check distance from player!

    auto distance = Vector3::Distance(pos, playerPos);


    if (distance > 30) 
    {
        return;
    }


    if (m_yPos == 0)
    {
        m_floorTerrain->Draw(deviceResources, m_accumulatedTransform);
    }


    RenderCubeTerrain(deviceResources, accumulatedTransform);

    // GetVertexDataFromGeometryShader();

    //m_streamOutThread = std::thread{ &MarchingCubesGeometryShader::GetVertexDataFromGeometryShader, this };
}

void MarchingCubesGeometryShader::RenderCubeTerrain(DX::DeviceResources & deviceResources, const DirectX::XMMATRIX &accumulatedTransform) const
{
    ID3D11ShaderResourceView* nullSRV[] = { nullptr };
    ID3D11Buffer* nullBuffer[] = { nullptr };

    // clear buffers and resources
    deviceResources.GetD3DDeviceContext()->PSSetConstantBuffers(0, 1, nullBuffer);
    deviceResources.GetD3DDeviceContext()->GSSetShaderResources(0, 1, nullSRV);

    m_accumulatedTransform = accumulatedTransform;

    // first render pass - build the density volume
    BuildDensityVolumeRenderPass(deviceResources);

    //m_quad->Draw(deviceResources);

    // second render pass - generate the vertices
    GenerateVerticesRenderPass(deviceResources);

    // clear the geometry shader
    deviceResources.GetD3DDeviceContext()->GSSetShader(0, 0, 0);
}

void MarchingCubesGeometryShader::BuildDensityVolumeRenderPass(DX::DeviceResources & deviceResources) const
{
    for (auto& b : m_densityVolumeRenderPass)
    {
        b->Bind(deviceResources);
    }

    deviceResources.GetD3DDeviceContext()->DrawInstanced(6, m_dimention, 0, 0);

    // reset the render target
    deviceResources.GetD3DDeviceContext()->OMSetRenderTargets(1, deviceResources.GetRenderTargetViewAddress(), deviceResources.GetDepthStencilView());
}

void MarchingCubesGeometryShader::GenerateVerticesRenderPass(DX::DeviceResources & deviceResources) const
{
    auto srv = m_buildDensitiesRT->GetShaderResourceViewAddress();
    deviceResources.GetD3DDeviceContext()->GSSetShaderResources(0, 1, srv);

    for (auto& b : m_generateVertsRenderPass)
    {
        b->Bind(deviceResources);
    }

    deviceResources.GetD3DDeviceContext()->GSSetSamplers(0, 1, m_gsSampler->GetAddressOf());

    // draw the object
    deviceResources.GetD3DDeviceContext()->DrawInstanced(m_generateVertexCount, m_dimention - 1, 0, 0);
}

void MarchingCubesGeometryShader::Update()
{
    auto playerPos = m_playerCamera->getPosition(); /// check distance from player!

    auto size = (m_dimention - 1) * m_scale;

    // check to see if player is inside the cube

    if (playerPos.x < m_worldPosition.x) return; 
    if (playerPos.z < m_worldPosition.z) return;
    if (playerPos.y < m_worldPosition.y) return; 

    if (playerPos.z > (m_worldPosition.z + size)) return; 
    if (playerPos.x > (m_worldPosition.x + size)) return; 
    if (playerPos.y > (m_worldPosition.y + size)) return; 


    // player is inside the cube

    auto playerX = (playerPos.x / m_scale) - (float)(m_dimention - 1) / 2.0f;
    auto playerY = -((playerPos.y / m_scale) - (float)(m_dimention - 1) / 2.0f);
    auto playerZ = (playerPos.z / m_scale) + 1; 

    m_playerCamera->playerDensityPosition = Vector3(playerX, playerY, playerZ);

    if (ImGui::Begin("Cube Collision"))
    {
        ImGui::Text("Player in cube x:%f, y:%f, z:%f", m_worldPosition.x, m_worldPosition.y, m_worldPosition.z);

        ImGui::Text("Player world pos x:%f, y:%f, z:%f", playerX, playerY, playerZ);
    }

    ImGui::End();
}

void MarchingCubesGeometryShader::GetVertexDataFromGeometryShader() const
{
    if (!m_vertexData.empty()) return;

    auto output = m_streamOutput->GetOutputBuffer();

    D3D11_BUFFER_DESC bufferDesc;

    output->GetDesc(&bufferDesc);

    bufferDesc.BindFlags = 0;
    bufferDesc.Usage = D3D11_USAGE_STAGING;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

    Microsoft::WRL::ComPtr<ID3D11Buffer> stagingBuffer;

    DX::ThrowIfFailed(m_deviceResources.GetD3DDevice()->CreateBuffer(&bufferDesc, nullptr, stagingBuffer.GetAddressOf()));

    m_deviceResources.GetD3DDeviceContext()->CopyResource(stagingBuffer.Get(), output);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    GSOutput* vertexData;

    m_deviceResources.GetD3DDeviceContext()->Map(stagingBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);

    auto arraySize = (int)pow((m_dimention - 1), 3) * 15;

    vertexData = (GSOutput*)mappedResource.pData;

    //std::vector<GSOutput> test(arraySize);

    //copy(vertexData, vertexData + arraySize, test.begin());

    //m_vertexData = test;
}
