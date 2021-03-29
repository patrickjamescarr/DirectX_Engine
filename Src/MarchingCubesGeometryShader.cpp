#include "pch.h"
#include "MarchingCubesGeometryShader.h"
#include "GeometryShader.h"
#include "InputLayout.h"
#include "Topology.h"
#include "DepthStencilState.h"
#include "TransformConstantBuffer.h"
#include "RasterizerState.h"
#include "Sampler.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

MarchingCubesGeometryShader::MarchingCubesGeometryShader(
    DX::DeviceResources & deviceResources,
    Light * sceneLight,
    SimpleMath::Matrix transform
)
    : Mesh(transform)
{
    int dims = 2;

    std::vector<VertexPosition> densityVerts;
    std::vector<VertexPosition> generationVerts;

    std::vector<uint32_t> indices;

    int index = 0;

    for (int j = 0; j < dims; j++)
    {
        for (int k = 0; k < dims; k++)
        {
            for (int i = 0; i < dims; i++)
            {
                VertexPosition v;

                v.position = Vector3(i, j, k);

                densityVerts.push_back(v);

                if (i < (dims - 1) && j < (dims - 1) && k < (dims - 1))
                {
                    generationVerts.push_back(v);
                }

                indices.push_back(index);
                index++;
            }
        }
    }

    // Create the index buffer and store a pointer to it
    auto indexBuffer = std::make_unique<IndexBuffer>(deviceResources, indices);
    m_indexBuffer = indexBuffer.get();
    AddIndexBuffer(std::move(indexBuffer));

    // configure density volume render pass
    {
        m_densityVolumeRenderPass.push_back(std::make_unique<Topology>(deviceResources, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST));
        m_densityVolumeRenderPass.push_back(std::make_unique<DepthStencilState>(deviceResources, DepthStencilState::default));
        m_densityVolumeRenderPass.push_back(std::make_unique<TransformConstantBuffer>(deviceResources, *this));

        // create the vertex buffer and store a pointer to it
        auto vertexBuffer = std::make_unique<VertexBuffer<VertexPosition>>(deviceResources, densityVerts);
        m_densityVertexBuffer = vertexBuffer.get();
        m_densityVolumeRenderPass.push_back(std::move(vertexBuffer));

        CD3D11_TEXTURE3D_DESC densityVolTexDesc;
        densityVolTexDesc.Height = 64;
        densityVolTexDesc.Width = 64;
        densityVolTexDesc.Depth = 64;
        densityVolTexDesc.MipLevels = 1;
        densityVolTexDesc.Format = DXGI_FORMAT_R16_FLOAT;
        densityVolTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        densityVolTexDesc.Usage = D3D11_USAGE_DEFAULT;
        densityVolTexDesc.CPUAccessFlags = 0;
        densityVolTexDesc.MiscFlags = 0;

        auto rt = std::make_unique<RenderTarget>(deviceResources, densityVolTexDesc);

        m_buildDensitiesRT = rt.get();

        m_densityVolumeRenderPass.push_back(std::move(rt));
        m_densityVolumeRenderPass.push_back(std::make_unique<PixelShader>(deviceResources, L"mc_build_densities_ps.cso"));

        auto vs = std::make_unique<VertexShader>(deviceResources, L"mc_build_densities_vs.cso");
        auto vsBytecode = vs->GetBytecode();

        // Create the vertex input layout description.
        std::vector<D3D11_INPUT_ELEMENT_DESC> layout{
            { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        // Create the input layout
        m_densityVolumeRenderPass.push_back(std::make_unique<InputLayout>(deviceResources, layout, vsBytecode));

        m_densityVolumeRenderPass.push_back(std::move(vs));
    }

    // configure generate verticies render pass
    {
        m_generateVertsRenderPass.push_back(std::make_unique<Topology>(deviceResources, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST));
        m_generateVertsRenderPass.push_back(std::make_unique<DepthStencilState>(deviceResources, DepthStencilState::default));
        m_generateVertsRenderPass.push_back(std::make_unique<TransformConstantBuffer>(deviceResources, *this));

        // create the vertex buffer and store a pointer to it
        auto vertexBuffer = std::make_unique<VertexBuffer<VertexPosition>>(deviceResources, generationVerts);
        m_generateVertsVertexBuffer = vertexBuffer.get();
        m_generateVertsRenderPass.push_back(std::move(vertexBuffer));

        m_gsSampler = std::make_unique<Sampler>(deviceResources, D3D11_TEXTURE_ADDRESS_CLAMP);

        m_generateVertsRenderPass.push_back(std::make_unique<GeometryShader>(deviceResources, L"mc_generate_vertices_gs.cso"));

        m_generateVertsRenderPass.push_back(std::make_unique<PixelShader>(deviceResources, L"mc_generate_vertices_ps.cso"));

        m_generateVertsRenderPass.push_back(std::make_unique<RasterizerState>(deviceResources, D3D11_CULL_NONE)); //D3D11_FILL_WIREFRAME

        auto vs = std::make_unique<VertexShader>(deviceResources, L"mc_generate_vertices_vs.cso");
        auto vsBytecode = vs->GetBytecode();

        // Create the vertex input layout description.
        std::vector<D3D11_INPUT_ELEMENT_DESC> layout{
            { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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
    m_accumulatedTransform = accumulatedTransform;

    // first render pass - build the density volume

    for (auto& b : m_densityVolumeRenderPass)
    {
        b->Bind(deviceResources);
    }

    deviceResources.GetD3DDeviceContext()->DrawIndexed(m_indexBuffer->GetCount(), 0, 0);


    // second render pass - generate the vertices

    // reset the render target
    auto renderTarget = deviceResources.GetRenderTargetView();
    auto depthStencil = deviceResources.GetDepthStencilView();
    deviceResources.GetD3DDeviceContext()->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // set the shader resource view from the first pass as the PS resource
    auto srv = m_buildDensitiesRT->GetShaderResourceView();
    deviceResources.GetD3DDeviceContext()->PSSetShaderResources(0, 1, &srv);

    for (auto& b : m_generateVertsRenderPass)
    {
        b->Bind(deviceResources);
    }

    deviceResources.GetD3DDeviceContext()->GSSetSamplers(0, 1, m_gsSampler->GetAddressOf());


    // draw the object
    deviceResources.GetD3DDeviceContext()->DrawIndexed(m_indexBuffer->GetCount(), 0, 0);


    // clear the geometry shader
    deviceResources.GetD3DDeviceContext()->GSSetShader(0, 0, 0);
}
