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
    int dims = 10;

    m_densityeVertexCount = 0;

    // 2d texure test
    {
        std::vector<VertexPositionTexture> verts;

        VertexPositionTexture topLeft;
        topLeft.position = Vector3(0, dims, 0.5f);
        topLeft.textureCoordinate = Vector2(0, 0);

        VertexPositionTexture topRight;
        topRight.position = Vector3(dims, dims, 0.5f);
        topRight.textureCoordinate = Vector2(1, 0);

        VertexPositionTexture bottomLeft;
        bottomLeft.position = Vector3(0, 0, 0.5f);
        bottomLeft.textureCoordinate = Vector2(0, 1);

        VertexPositionTexture bottomRight;
        bottomRight.position = Vector3(dims, 0, 0.5f);
        bottomRight.textureCoordinate = Vector2(1, 1);

        verts.push_back(topLeft);
        verts.push_back(bottomRight);
        verts.push_back(bottomLeft);
        verts.push_back(topLeft);
        verts.push_back(topRight);
        verts.push_back(bottomRight);

        std::vector<uint32_t> indices;

        for (int i = 0; i < 6; i++)
        {
            indices.push_back(i);
        }

        m_texture2dTestPass.push_back(std::make_unique<Topology>(deviceResources, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
        m_texture2dTestPass.push_back(std::make_unique<DepthStencilState>(deviceResources, DepthStencilState::default));
        m_texture2dTestPass.push_back(std::make_unique<MarchingCubesConstantBufferVS>(deviceResources, *this, dims));

        m_texture2dTestPass.push_back(std::make_unique<VertexBuffer<VertexPositionTexture>>(deviceResources, verts));

        CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R16_FLOAT, dims, dims,
            1, 1, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

        // 3D texture description for storing the density values
        CD3D11_TEXTURE3D_DESC densityVolTexDesc;
        densityVolTexDesc.Height = dims;
        densityVolTexDesc.Width = dims;
        densityVolTexDesc.Depth = dims;
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
        // pull out the vertex shader bytecode
        auto vsBytecode = vs->GetBytecode();
        m_texture2dTestPass.push_back(std::move(vs));

        // geometry shader
        m_texture2dTestPass.push_back(std::make_unique<GeometryShader>(deviceResources, L"mc_build_densities_gs.cso"));

        // pixel shader
        m_texture2dTestPass.push_back(std::make_unique<PixelShader>(deviceResources, L"mc_build_densities_ps.cso"));


        // Create the vertex input layout description.
        std::vector<D3D11_INPUT_ELEMENT_DESC> layout{
            { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                               D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT,    D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };

        // Create the input layout
        m_texture2dTestPass.push_back(std::make_unique<InputLayout>(deviceResources, layout, vsBytecode));
    }

    // configure generate verticies render pass
    {
        std::vector<VertexPositionTexture> generationVerts;
        m_generateVertexCount = 0;

        float textureCoordinatesStep = 1.0f / dims;

        for (int j = 0; j < (dims - 1); j++)
        {
            for (int k = 0; k < (dims - 1); k++)
            {
                for (int i = 0; i < (dims - 1); i++)
                {
                    VertexPositionTexture v;

                    v.position = Vector3(i, j, k);
                    v.textureCoordinate = Vector2((float)i * textureCoordinatesStep, (float)j*textureCoordinatesStep);

                    generationVerts.push_back(v);

                    m_generateVertexCount++;
                }
            }
        }

        m_generateVertsRenderPass.push_back(std::make_unique<Topology>(deviceResources, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST));
        m_generateVertsRenderPass.push_back(std::make_unique<DepthStencilState>(deviceResources, DepthStencilState::default));

        m_generateVertsRenderPass.push_back(std::make_unique<MarchingCubesConstantBufferGS>(deviceResources, *this, &m_isoLevel));
        m_generateVertsRenderPass.push_back(std::make_unique<TransformConstantBufferVS>(deviceResources, *this));
        //m_generateVertsRenderPass.push_back(std::make_unique<MarchingCubesConstantBufferVS>(deviceResources, *this, dims));

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
            { "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT,    D3D11_INPUT_PER_VERTEX_DATA, 0 }
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

    m_accumulatedTransform = accumulatedTransform;

    // first render pass - build the density volume
    deviceResources.GetD3DDeviceContext()->PSSetConstantBuffers(0, 1, nullBuffer);
    deviceResources.GetD3DDeviceContext()->GSSetShaderResources(0, 1, nullSRV);

    for (auto& b : m_texture2dTestPass)
    {
        b->Bind(deviceResources);
    }

    deviceResources.GetD3DDeviceContext()->DrawInstanced(6, 10, 0, 0);

    // reset the render target
    deviceResources.GetD3DDeviceContext()->OMSetRenderTargets(1, deviceResources.GetRenderTargetViewAddress(), deviceResources.GetDepthStencilView());

    // second render pass - generate the vertices

    // set the shader resource view from the first pass as the PS resource
    //auto srv = m_buildDensitiesRT->GetShaderResourceViewAddress();
    auto srv = m_texture2dRT->GetShaderResourceViewAddress();
    deviceResources.GetD3DDeviceContext()->GSSetShaderResources(0, 1, srv);

    for (auto& b : m_generateVertsRenderPass)
    {
        b->Bind(deviceResources);
    }

    deviceResources.GetD3DDeviceContext()->GSSetSamplers(0, 1, m_gsSampler->GetAddressOf());


    // draw the object
    deviceResources.GetD3DDeviceContext()->DrawInstanced(m_generateVertexCount, 1, 0, 0);


    // clear the geometry shader
    deviceResources.GetD3DDeviceContext()->GSSetShader(0, 0, 0);
}

void MarchingCubesGeometryShader::Update()
{
    if (ImGui::Begin("Marching Cubes"))
    {
        ImGui::SliderFloat("Iso Level", &m_isoLevel, 0.0f, 10.0f);
    }

    ImGui::End();
}
