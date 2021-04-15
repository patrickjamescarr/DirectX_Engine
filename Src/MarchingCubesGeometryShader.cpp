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

using namespace DirectX;
using namespace DirectX::SimpleMath;

MarchingCubesGeometryShader::MarchingCubesGeometryShader(
    DX::DeviceResources & deviceResources,
    Light * sceneLight,
    Camera* activeCamera,
    SimpleMath::Matrix transform
)
    : Mesh(transform)
{
    m_densityeVertexCount = 0;

    // 2d texure test
    {
        auto scaledDim = m_dimention;

        float size = (float)m_dimention / 2.0f;

        std::vector<VertexPositionTexture> verts;
        verts.push_back(VertexPositionTexture(Vector3(-size, size, 0.5f), Vector2(0, 0)));   // top left
        verts.push_back(VertexPositionTexture(Vector3(size, size, 0.5f), Vector2(0, 1)));     // top right
        verts.push_back(VertexPositionTexture(Vector3(size, -size, 0.5f), Vector2(1, 1)));     // bottom right

        verts.push_back(VertexPositionTexture(Vector3(size, -size, 0.5f), Vector2(1, 1)));     // bottom right
        verts.push_back(VertexPositionTexture(Vector3(-size, -size, 0.5f), Vector2(1, 0)));     // bottom left
        verts.push_back(VertexPositionTexture(Vector3(-size, size, 0.5f), Vector2(0, 0)));   // top left

        m_texture2dTestPass.push_back(std::make_unique<Topology>(deviceResources, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
        m_texture2dTestPass.push_back(std::make_unique<DepthStencilState>(deviceResources, DepthStencilState::default));
        m_texture2dTestPass.push_back(std::make_unique<MarchingCubesConstantBufferVS>(deviceResources, *this, m_dimention));
        m_texture2dTestPass.push_back(std::make_unique<RasterizerState>(deviceResources, D3D11_CULL_NONE));
        m_texture2dTestPass.push_back(std::make_unique<VertexBuffer<VertexPositionTexture>>(deviceResources, verts));

        auto width = deviceResources.GetBackBufferWidth();
        auto height = deviceResources.GetBackBufferHeight();

        CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R16_FLOAT, m_dimention, m_dimention,
            1, 1, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

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
        m_texture2dRT = rt.get();
        m_texture2dTestPass.push_back(std::move(rt));

        // create the vertex shader
        auto vs = std::make_unique<VertexShader>(deviceResources, L"mc_build_densities_vs.cso");
        //auto vs = std::make_unique<VertexShader>(deviceResources, L"map_vs.cso");
        // pull out the vertex shader bytecode
        auto vsBytecode = vs->GetBytecode();
        m_texture2dTestPass.push_back(std::move(vs));

        // geometry shader
        m_texture2dTestPass.push_back(std::make_unique<GeometryShader>(deviceResources, L"mc_build_densities_gs.cso"));

        // pixel shader
        m_texture2dTestPass.push_back(std::make_unique<PixelShader>(deviceResources, L"mc_build_densities_ps.cso"));
        //m_texture2dTestPass.push_back(std::make_unique<PixelShader>(deviceResources, L"map_ps.cso"));

        // Create the vertex input layout description.
        std::vector<D3D11_INPUT_ELEMENT_DESC> layout{
            { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                               D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT,    D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        // Create the input layout
        m_texture2dTestPass.push_back(std::make_unique<InputLayout>(deviceResources, layout, vsBytecode));
    }
    // Matrix::CreateTranslation(3.5, 2.5, 0)
    //m_quad = std::make_unique<OnScreenQuad>(deviceResources, 100, 100, Matrix::CreateTranslation(1.5, 1.5, 0), m_texture2dRT);

    // configure generate verticies render pass
    {
        std::vector<VertexPositionTexture> generationVerts;
        m_generateVertexCount = 0;

        float textureCoordinatesStep = 1.0f / m_dimention;

        TerrainDensityFunction density(nullptr);

        for (int j = 0; j < (m_dimention - 1); j++)
        {
            for (int i = 0; i < (m_dimention - 1); i++)
            {
                VertexPositionTexture v;

                v.position = Vector3(i, j, 1) * 0.03;
                v.textureCoordinate = Vector2((float)i * textureCoordinatesStep, (float)j * textureCoordinatesStep);
                generationVerts.push_back(v);

                m_generateVertexCount++;
            }
        }

        m_generateVertsRenderPass.push_back(std::make_unique<Topology>(deviceResources, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST));
        m_generateVertsRenderPass.push_back(std::make_unique<DepthStencilState>(deviceResources, DepthStencilState::default));

        m_generateVertsRenderPass.push_back(std::make_unique<MarchingCubesConstantBufferGS>(deviceResources, *this, &m_isoLevel));
        m_generateVertsRenderPass.push_back(std::make_unique<TransformConstantBufferVS>(deviceResources, *this));
        //m_generateVertsRenderPass.push_back(std::make_unique<MarchingCubesConstantBufferVS>(deviceResources, *this, m_dimention));

        m_generateVertsRenderPass.push_back(std::make_unique<CameraConstantBuffer>(deviceResources, activeCamera, 1));
        m_generateVertsRenderPass.push_back(std::make_unique<LightConstantBuffer>(deviceResources, sceneLight));

        // create the vertex buffer and store a pointer to it
        auto vertexBuffer = std::make_unique<VertexBuffer<VertexPositionTexture>>(deviceResources, generationVerts);
        m_generateVertsVertexBuffer = vertexBuffer.get();
        m_generateVertsRenderPass.push_back(std::move(vertexBuffer));

        m_gsSampler = std::make_unique<Sampler>(deviceResources, D3D11_TEXTURE_ADDRESS_CLAMP);

        m_generateVertsRenderPass.push_back(std::make_unique<RasterizerState>(deviceResources, D3D11_CULL_NONE)); //D3D11_FILL_WIREFRAME

        m_generateVertsRenderPass.push_back(std::make_unique<GeometryShader>(deviceResources, L"mc_generate_vertices_gs.cso"));

        m_generateVertsRenderPass.push_back(std::make_unique<PixelShader>(deviceResources, L"mc_generate_vertices_ps.cso"));

        auto vs = std::make_unique<VertexShader>(deviceResources, L"mc_generate_vertices_vs.cso");

        auto vsBytecode = vs->GetBytecode();

        // Create the vertex input layout description.
        std::vector<D3D11_INPUT_ELEMENT_DESC> layout{
            { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                               D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT,    D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        // Create the input layout
        m_generateVertsRenderPass.push_back(std::make_unique<InputLayout>(deviceResources, layout, vsBytecode));

        m_generateVertsRenderPass.push_back(std::move(vs));
    }
}

void MarchingCubesGeometryShader::Draw(
    DX::DeviceResources & deviceResources,
    FXMMATRIX accumulatedTransform
) const noexcept
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
    for (auto& b : m_texture2dTestPass)
    {
        b->Bind(deviceResources);
    }

    deviceResources.GetD3DDeviceContext()->DrawInstanced(6, m_dimention, 0, 0);

    // reset the render target
    deviceResources.GetD3DDeviceContext()->OMSetRenderTargets(1, deviceResources.GetRenderTargetViewAddress(), deviceResources.GetDepthStencilView());
}

void MarchingCubesGeometryShader::GenerateVerticesRenderPass(DX::DeviceResources & deviceResources) const
{
    auto srv = m_texture2dRT->GetShaderResourceViewAddress();
    deviceResources.GetD3DDeviceContext()->GSSetShaderResources(0, 1, srv);

    for (auto& b : m_generateVertsRenderPass)
    {
        b->Bind(deviceResources);
    }

    deviceResources.GetD3DDeviceContext()->GSSetSamplers(0, 1, m_gsSampler->GetAddressOf());


    // draw the object
    deviceResources.GetD3DDeviceContext()->DrawInstanced(m_generateVertexCount, m_dimention, 0, 0);
}

void MarchingCubesGeometryShader::Update()
{
    if (ImGui::Begin("Marching Cubes"))
    {
        ImGui::SliderFloat("Iso Level", &m_isoLevel, 0.0f, 10.0f);
    }

    ImGui::End();
}
