#include "pch.h"
#include "MarchingCubesTerrainBlock.h"
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

///
/// Marching Cubes Geometry Shader 
///
/// Performs 2 render passes to create an n^3 dimention section of procedural marching cubes terrain
///
/// 1st pass -  renders a 3D density volume to texture using a density function 
///             defined in shader code. Density is calculated in the pixel shader 
///             and defined by the position of the section of terrain.
///
/// 2nd pass -  generates the vertices of the terrain in the geometry shader using the marching
///             cubes algorithm. the algorithm samples the 3D density texture to determine where
///             geometry is to be placed.
///

using namespace DirectX;
using namespace DirectX::SimpleMath;

MarchingCubesTerrainBlock::MarchingCubesTerrainBlock(
    DX::DeviceResources & deviceResources,
    Light * sceneLight,
    Camera* playerCamera,
    ViewingFrustum* viewingFrustum,
    ID3D11RenderTargetView ** bloomRenderTarget,
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
    m_playerCamera(playerCamera), m_scale(scale), m_yPos(yPos), m_fogEnd(fogEnd), m_deviceResources(deviceResources),
    m_bloomRenderTarget(bloomRenderTarget)
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

        // create the viewing frustum transform. used for culling sections out of view
        m_viewingFrustumTransform =
            Matrix::CreateScale(m_scale) * Matrix::CreateTranslation(
                halfBlock + (xIncrement * m_scale),
                halfBlock + (yIncrement * m_scale),
                halfBlock + (zIncrement * m_scale)
            );

        m_cubeRadius = sqrtf(pow(halfBlock, 2.0f) + pow(halfBlock, 2.0f));

        m_position = Vector3(xPos, yPos, zPos);
        m_worldPosition = Vector3(xPos * scaledOffset, yPos * scaledOffset, zPos * scaledOffset);

        // generate vertex and texture positions for the quad used to create each slice of the density volume
        std::vector<VertexPositionTexture> verts;
        verts.push_back(VertexPositionTexture(Vector3(-size + xIncrement, size - yIncrement, zIncrement), Vector2(0, 0)));  // top left
        verts.push_back(VertexPositionTexture(Vector3(size + xIncrement, size - yIncrement, zIncrement), Vector2(0, 1)));   // top right
        verts.push_back(VertexPositionTexture(Vector3(size + xIncrement, -size - yIncrement, zIncrement), Vector2(1, 1)));  // bottom right

        verts.push_back(VertexPositionTexture(Vector3(size + xIncrement, -size - yIncrement, zIncrement), Vector2(1, 1)));  // bottom right
        verts.push_back(VertexPositionTexture(Vector3(-size + xIncrement, -size - yIncrement, zIncrement), Vector2(1, 0))); // bottom left
        verts.push_back(VertexPositionTexture(Vector3(-size + xIncrement, size - yIncrement, zIncrement), Vector2(0, 0)));  // top left

        //create the vertex buffer
        m_densityVolumeRenderPass.push_back(std::make_unique<VertexBuffer<VertexPositionTexture>>(deviceResources, verts));

        // vertex shader constant buffer
        m_densityVolumeRenderPass.push_back(std::make_unique<MarchingCubesDensityConstantBufferVS>(deviceResources, transform, m_dimention, xPos, yPos));

        // Create standard topology, depth stencil and rasterizer state
        m_densityVolumeRenderPass.push_back(std::make_unique<Topology>(deviceResources, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
        m_densityVolumeRenderPass.push_back(std::make_unique<DepthStencilState>(deviceResources));
        m_densityVolumeRenderPass.push_back(std::make_unique<RasterizerState>(deviceResources, D3D11_CULL_BACK));

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

        // create the render target for the density volume texture
        auto rt = std::make_unique<RenderTarget>(deviceResources, densityVolTexDesc);
        m_buildDensitiesRT = rt.get();
        m_densityVolumeRenderPass.push_back(std::move(rt));

        // geometry shader
        m_densityVolumeRenderPass.push_back(std::make_unique<GeometryShader>(deviceResources, L"mc_build_densities_gs.cso"));

        // pixel shader
        m_densityVolumeRenderPass.push_back(std::make_unique<PixelShader>(deviceResources, L"mc_build_densities_ps.cso"));

        // create the vertex shader
        auto vs = std::make_unique<VertexShader>(deviceResources, L"mc_build_densities_vs.cso");

        // pull out the vertex shader bytecode
        auto vsBytecode = vs->GetBytecode();
        m_densityVolumeRenderPass.push_back(std::move(vs));

        // Create the vertex input layout description.
        std::vector<D3D11_INPUT_ELEMENT_DESC> layout(std::begin(VertexPositionTexture::InputElements), std::end(VertexPositionTexture::InputElements));

        // Create the input layout
        m_densityVolumeRenderPass.push_back(std::make_unique<InputLayout>(deviceResources, layout, vsBytecode));
    }

    // configure generate verticies render pass
    {
        std::vector<VertexPositionTexture> generationVerts;
        m_generateVertexCount = 0;

        float textureCoordinatesStep = 1.0f / ((float)m_dimention - 1.0f);

        for (int j = 0; j < (m_dimention - 1); j++)
        {
            for (int i = 0; i < (m_dimention - 1); i++)
            {
                VertexPositionTexture v;

                v.position = Vector3((float)i * m_scale, (float)j * m_scale, 1) ;
                v.textureCoordinate = Vector2((float)i * textureCoordinatesStep, (float)j * textureCoordinatesStep);
                generationVerts.push_back(v);

                m_generateVertexCount++;
            }
        }

        // Create point list topology, depth stencil and rasterizer state
        m_generateVertsRenderPass.push_back(std::make_unique<Topology>(deviceResources, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST));
        m_generateVertsRenderPass.push_back(std::make_unique<DepthStencilState>(deviceResources));
        m_generateVertsRenderPass.push_back(std::make_unique<RasterizerState>(deviceResources, D3D11_CULL_BACK)); //D3D11_FILL_WIREFRAME
        
        // constant buffers
        //vertex
        m_generateVertsRenderPass.push_back(std::make_unique<MarchingCubesGenerateConstantBufferVS>(deviceResources, &m_dimention, &m_scale, 1));
        m_generateVertsRenderPass.push_back(std::make_unique<TransformConstantBufferVS>(deviceResources, *this));
        // geometry
        m_generateVertsRenderPass.push_back(std::make_unique<MarchingCubesConstantBufferGS>(deviceResources, *this, m_isoLevel, &m_dimention));
        m_generateVertsRenderPass.push_back(std::make_unique<FogConstantBuffer>(deviceResources, m_fogEnd, playerCamera, ShaderType::Geometry, 3));
        //pixel
        m_generateVertsRenderPass.push_back(std::make_unique<LightConstantBuffer>(deviceResources, sceneLight));

        // create the vertex buffer and store a pointer to it
        m_generateVertsRenderPass.push_back(std::make_unique<VertexBuffer<VertexPositionTexture>>(deviceResources, generationVerts));

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
    }
}

void MarchingCubesTerrainBlock::Draw(
    DX::DeviceResources & deviceResources,
    FXMMATRIX accumulatedTransform
) const noexcept
{
    m_accumulatedTransform = accumulatedTransform;

    Vector3 pos = Vector3::Transform(m_position, m_accumulatedTransform * m_viewingFrustumTransform);

    // cull sections outwith the current view frustum
    bool draw = m_frustum->CheckCube(pos.x, pos.y, pos.z, m_cubeRadius);

    if (!draw)
    {
        return;
    }

    // cull sections outwith a certain range to save on draw calls
    auto playerPos = m_playerCamera->getPosition();
    auto distance = Vector3::Distance(pos, playerPos);

    if (distance > 33) 
    {
        return;
    }

    // only draw the floor on the bottom layer
    if (m_yPos == 0)
    {
        m_floorTerrain->Draw(deviceResources, m_accumulatedTransform);
    }

    // safe to render
    RenderCubeTerrain(deviceResources, accumulatedTransform);
}

void MarchingCubesTerrainBlock::RenderCubeTerrain(DX::DeviceResources & deviceResources, const DirectX::XMMATRIX &accumulatedTransform) const
{
    ID3D11ShaderResourceView* nullSRV[] = { nullptr };
    ID3D11Buffer* nullBuffer[] = { nullptr };

    // clear buffers and resources
    deviceResources.GetD3DDeviceContext()->PSSetConstantBuffers(0, 1, nullBuffer);
    deviceResources.GetD3DDeviceContext()->GSSetShaderResources(0, 1, nullSRV);

    m_accumulatedTransform = accumulatedTransform;

    // first render pass - build the density volume
    BuildDensityVolumeRenderPass(deviceResources);

    // second render pass - generate the vertices
    GenerateVerticesRenderPass(deviceResources);

    // clear the geometry shader
    deviceResources.GetD3DDeviceContext()->GSSetShader(0, 0, 0);
}

void MarchingCubesTerrainBlock::BuildDensityVolumeRenderPass(DX::DeviceResources & deviceResources) const
{
    // bind the resources for this pass
    for (auto& b : m_densityVolumeRenderPass)
    {
        b->Bind(deviceResources);
    }

    // draw n instances of the density render pass to build up the 3D density volume
    deviceResources.GetD3DDeviceContext()->DrawInstanced(6, m_dimention, 0, 0);

    // reset the render target
    deviceResources.GetD3DDeviceContext()->OMSetRenderTargets(1, m_bloomRenderTarget, deviceResources.GetDepthStencilView());
}

void MarchingCubesTerrainBlock::GenerateVerticesRenderPass(DX::DeviceResources & deviceResources) const
{
    // set the GS SRV to the 3D density volume created in the first pass.
    // this is used by the marching cubes algorithm to determine where the geometry is located
    auto srv = m_buildDensitiesRT->GetShaderResourceViewAddress();
    deviceResources.GetD3DDeviceContext()->GSSetShaderResources(0, 1, srv);

    // set the geometry shader sampler
    deviceResources.GetD3DDeviceContext()->GSSetSamplers(0, 1, m_gsSampler->GetAddressOf());

    // bind the rest of the resources for this pass
    for (auto& b : m_generateVertsRenderPass)
    {
        b->Bind(deviceResources);
    }

    // draw the geometry
    deviceResources.GetD3DDeviceContext()->DrawInstanced(m_generateVertexCount, m_dimention - 1, 0, 0);
}

void MarchingCubesTerrainBlock::Update()
{
    // perform collision detection to see if the player is inside this cube
    // if so, calculate the players positon relative to the 3D density volume
    // for this section for performing collision detection on the terrain.

    auto playerPos = m_playerCamera->getPosition();

    auto size = (m_dimention - 1) * m_scale;

    // check to see if player is inside the cube

    if (playerPos.x < m_worldPosition.x) return; 
    if (playerPos.z < m_worldPosition.z) return;
    if (playerPos.y < m_worldPosition.y) return; 

    if (playerPos.z > (m_worldPosition.z + size)) return; 
    if (playerPos.x > (m_worldPosition.x + size)) return; 
    if (playerPos.y > (m_worldPosition.y + size)) return; 


    // player is inside the cube. calculate the position relative to the density texture
    // for this section to determine collision detection with the terrain geometry
    auto playerX = (playerPos.x / m_scale) - (float)(m_dimention - 1) / 2.0f;
    auto playerY = -((playerPos.y / m_scale) - (float)(m_dimention - 1) / 2.0f);
    auto playerZ = (playerPos.z / m_scale) + 1; 

    // set the density coordinate
    m_playerCamera->playerDensityPosition = Vector3(playerX, playerY, playerZ);
}
